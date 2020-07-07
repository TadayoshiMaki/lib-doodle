#include "HsvSensorProd.h"

#include "StApi_IP.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

SentechHsvSensor::SentechHsvSensor() try : HsvSensor(), lib_()
{
}
catch (const GenICam::GenericException& e)
{
    // TODO
}

SentechHsvSensor::~SentechHsvSensor()
{
}

uint32_t SentechHsvSensor::getWidth()
{
    return imageWidth_;
}

uint32_t SentechHsvSensor::getHeight()
{
    return imageHeight_;
}

HsvSensor::PixelFormat SentechHsvSensor::getPixelFormat()
{
    switch (imagePixelFormat_)
    {
    case EStPixelFormatNamingConvention_t::StPFNC_Mono8:
        return PixelFormat::GRAY;
    case EStPixelFormatNamingConvention_t::StPFNC_BayerRG8:
        // Acquired images will be converted to RGB format.
    default:
        // TODO: Handle currently unknown pixel formats.
        return PixelFormat::RGB;
    }
}

bool SentechHsvSensor::onOpenSensor(const std::shared_ptr<HsvSensor::Config>& config)
{
    using GenApi::CNodeMapPtr;
    using StApi::CreateIStSystem;
    using StApi::EStInterfaceType_t;
    using StApi::EStPixelFormatNamingConvention_t;
    using StApi::EStSystemVendor_t;

    auto sensorConfig = std::dynamic_pointer_cast<Config>(config);
    try
    {
        system_.Reset(CreateIStSystem(EStSystemVendor_t::StSystemVendor_Sentech,
                                      EStInterfaceType_t::StInterfaceType_All));
        device_.Reset(system_->CreateFirstIStDevice(GenTL::DEVICE_ACCESS_EXCLUSIVE));
        if (!device_) return false;

        stream_.Reset(device_->CreateIStDataStream(0));
    }
    catch (const GenICam::GenericException& e)
    {
        return false;
    }

    // Load the configuration file and hold some applied properties.
    bool success = loadConfiguration(sensorConfig->getConfigFilePath());
    if (!success) return false;
    try
    {
        CNodeMapPtr nodeMap(device_->GetRemoteIStPort()->GetINodeMap());
        imageWidth_ = getValue<int>(nodeMap, "Width");
        imageHeight_ = getValue<int>(nodeMap, "Height");
        int pixelFormat = getValue<int>(nodeMap, "PixelFormat");
        imagePixelFormat_ = static_cast<EStPixelFormatNamingConvention_t>(pixelFormat);
    }
    catch (const GenICam::GenericException& e)
    {
        return false;
    }

    return success;
}

void SentechHsvSensor::onCloseSensor()
{
    try
    {
        stream_.Reset();
        device_.Reset();
        system_.Reset();
    }
    catch (const GenICam::GenericException& e)
    {
        // TODO
    }
}

bool SentechHsvSensor::onStartSensor()
{
    try
    {
        stream_->StartAcquisition(GENTL_INFINITE);
        auto writable = device_->GetRemoteIStPort()->GetIStPortInfo()->IsAccessWrite();
        if (!writable) return false;
        device_->AcquisitionStart();
    }
    catch (const GenICam::GenericException& e)
    {
        return false;
    }
    return true;
}

bool SentechHsvSensor::onStopSensor()
{
    try
    {
        auto writable = device_->GetRemoteIStPort()->GetIStPortInfo()->IsAccessWrite();
        if (!writable) return false;
        device_->AcquisitionStop();
        stream_->StopAcquisition();
    }
    catch (const GenICam::GenericException& e)
    {
        return false;
    }
    return true;
}

void SentechHsvSensor::run()
{
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::system_clock;
    using GenApi::CNodeMapPtr;
    using StApi::CIStImageBufferPtr;
    using StApi::EStTimeoutHandling_t;

    auto lastFrameTime = system_clock::now();
    auto currentTime = system_clock::now();

    // TODO: Now handle only one window.
    const int regionIds[] = { 0 };
    std::vector<SensingResult> prevResults;
    CNodeMapPtr nodeMap(device_->GetRemoteIStPort()->GetINodeMap());
    while (started_) {
        if (!stream_->IsGrabbing()) continue;

        CIStStreamBufferPtr buffer(
            stream_->RetrieveBuffer(1, EStTimeoutHandling_t::StTimeoutHandling_Count));
        if (!buffer) continue;

        auto imageAvailable = buffer->GetIStStreamBufferInfo()->IsImagePresent();
        if (imageAvailable && captureEnabled_) onAcquireImage(buffer);

        currentTime = system_clock::now();
        auto currentTimeEpoch = currentTime.time_since_epoch();
        int64_t currentTimeMicros = duration_cast<microseconds>(currentTimeEpoch).count();

        std::vector<SensingResult> tempResults;
        for (auto&& regionId : regionIds)
        {
            bool selected = setValue(nodeMap, "ChunkDetectObjectRegionSelector", regionId);
            if (!selected) continue;
            bool detected = getValue<bool>(nodeMap, "ChunkDetectObjectResult");
            if (!detected) continue;

            int moment0 = getValue<int>(nodeMap, "ChunkDetectObjectMoment0");
            int moment1X = getValue<int>(nodeMap, "ChunkDetectObjectMoment1X");
            int moment1Y = getValue<int>(nodeMap, "ChunkDetectObjectMoment1Y");

            PointF point((float)moment1X / moment0, (float)moment1Y / moment0);
            PointF dstPoint(point);
            if (undistorter_)
            {
                dstPoint = *(undistorter_->undistort(point));
            }

            tempResults.emplace_back(SensingState::NONE, static_cast<unsigned char>(regionId),
                                     dstPoint.x,
                                     dstPoint.y,
                                     currentTimeMicros);
        }
        assignSensingState(tempResults, prevResults, currentTimeMicros);
        bool shouldCallbackNow = false;
        for (auto& tRes : tempResults) {
            if (tRes.sensingState == SensingState::DETECT || tRes.sensingState == SensingState::LOST) {
                shouldCallbackNow = true;
                break;
            }
        }
        results_ = std::move(tempResults);
        prevResults = results_;

        auto elapsedMicros = duration_cast<microseconds>(currentTime - lastFrameTime).count();
        if (elapsedMicros < callbackIntervalMicros_)
        {
            if (shouldCallbackNow && sensingResultCallback_)(*sensingResultCallback_)(results_);
            // TODO: Prevent from busy loop.
            continue;
        }
        lastFrameTime = currentTime;

        if (captureEnabled_)
        {
            if (captureCallback_) (*captureCallback_)(frame_);
        }

        if (sensingResultCallback_) (*sensingResultCallback_)(results_);
    }
}

bool SentechHsvSensor::loadConfiguration(const std::string& configFilePath)
{
    using StApi::CIStFeatureBagPtr;
    using StApi::CreateIStFeatureBag;
    using GenApi::CNodeMapPtr;

    bool alreadyStarted = started_;
    if (alreadyStarted) stop();
    try
    {
        CIStFeatureBagPtr setting = CreateIStFeatureBag();
        GenICam::gcstring filePath(configFilePath.c_str());
        setting->StoreFileToBag(filePath);
        CNodeMapPtr nodeMap(device_->GetRemoteIStPort()->GetINodeMap());
        setting->Load(nodeMap, true);
    }
    catch (const GenICam::GenericException& e)
    {
        return false;
    }

    if (alreadyStarted) start();
    return true;
}

void SentechHsvSensor::onAcquireImage(const CIStStreamBufferPtr& buffer)
{
    auto *image = buffer->GetIStImage();
    auto pixelFormat = image->GetImagePixelFormat();
    const auto* pixelFormatInfo = GetIStPixelFormatInfo(pixelFormat);
    if (pixelFormatInfo->IsBayer())
    {
        onAcquireColorImage(image);
    }
    else if (pixelFormatInfo->IsMono())
    {
        onAcquireMonoImage(image, *pixelFormatInfo);
    }
}

void SentechHsvSensor::onAcquireColorImage(IStImage* image)
{
    using StApi::CIStImageBufferPtr;
    using StApi::CreateIStImageBuffer;
    using StApi::CIStPixelFormatConverterPtr;
    using StApi::CreateIStConverter;
    using StApi::EStPixelFormatNamingConvention_t;
    using StApi::EStConverterType_t;

    CIStImageBufferPtr imageBuffer = CreateIStImageBuffer();
    CIStPixelFormatConverterPtr converter = CreateIStConverter(EStConverterType_t::StConverterType_PixelFormat);
    converter->SetDestinationPixelFormat(EStPixelFormatNamingConvention_t::StPFNC_RGB8);
    converter->Convert(image, imageBuffer);

    IStImage *converted = imageBuffer->GetIStImage();
    const size_t width = converted->GetImageWidth();
    const size_t height = converted->GetImageHeight();
    const int type = CV_8UC3;
    if (frame_->cols != width || (frame_->rows != height) || (frame_->type() != type))
    {
        frame_->create(height, width, type);
    }
    const size_t bufferSize = frame_->rows * frame_->cols * frame_->channels();
    memcpy(frame_->ptr(0), converted->GetImageBuffer(), bufferSize);
}

void SentechHsvSensor::onAcquireMonoImage(IStImage *image, const IStPixelFormatInfo& pixelFormatInfo)
{
    const size_t width = image->GetImageWidth();
    const size_t height = image->GetImageHeight();
    int type = CV_8UC1;
    if (frame_->cols != width || frame_->rows != height || frame_->type() != type)
    {
        frame_->create(height, width, type);
    }
    const size_t bufferSize = frame_->rows * frame_->cols * frame_->elemSize() * frame_->channels();
    memcpy(frame_->ptr(0), image->GetImageBuffer(), bufferSize);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
