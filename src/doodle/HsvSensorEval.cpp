#include "doodle/HsvSensorEval.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

SonyHsvSensor::SonyHsvSensor() : HsvSensor()
{
    camera_ = std::make_shared<HsvCam>();
}

SonyHsvSensor::~SonyHsvSensor()
{
}

uint32_t SonyHsvSensor::getWidth()
{
    return camera_->getCropWidth();
}

uint32_t SonyHsvSensor::getHeight()
{
    return camera_->getCropHeight();
}

HsvSensor::PixelFormat SonyHsvSensor::getPixelFormat()
{
    return PixelFormat::BGR;
}

bool SonyHsvSensor::onOpenSensor(const std::shared_ptr<HsvSensor::Config>& config)
{
    auto sensorConfig = std::dynamic_pointer_cast<Config>(config);
    if (!camera_->loadSetupFile(sensorConfig->getSetupFilePath()))
    {
        // Failed to load the setup file.
        return false;
    }

    if (!camera_->setup(sensorConfig->getParamsSubDirectory()))
    {
        // Failed to setup the camera with the parameters.
        return false;
    }
    return true;
}

void SonyHsvSensor::onCloseSensor()
{
    camera_->close();
}

bool SonyHsvSensor::onStartSensor()
{
    return camera_->startCapture();
}

bool SonyHsvSensor::onStopSensor()
{
    return camera_->stopCapture();
}

void SonyHsvSensor::run()
{
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::system_clock;

    SensingInformationPacket_t packet;
    uint32_t lastFrameCount = 0;
    auto lastFrameTime = system_clock::now();
    auto currentTime = system_clock::now();
    std::vector<SensingResult> prevResults;

    while (started_)
    {
        bool success = false;

        if (captureEnabled_)
        {
            success = camera_->getRgbImage(*frame_, packet);
        }
        else
        {
            success = camera_->getSensingInformation(packet);
        }

        // Failed to obtain data or ignore the duplicate packet.
        if (!success || lastFrameCount == packet.frameCount) continue;

        lastFrameCount = packet.frameCount;

        currentTime = std::chrono::system_clock::now();
        auto currentTimeEpoch = currentTime.time_since_epoch();
        auto currentTimeMicros = duration_cast<microseconds>(currentTimeEpoch).count();

        std::vector<SensingResult> tempResults;
        // TODO: Now handle only one window. Should handle all windows.
        auto trackingInfo = packet.win.at(0);
        // Ignore results which indicates the undetected state.
        if (trackingInfo.result)
        {
            PointF point((float)trackingInfo.moment1_x / trackingInfo.moment0,
                         (float)trackingInfo.moment1_y / trackingInfo.moment0);
            PointF dstPoint(point);
            if (undistorter_)
            {
                dstPoint = *(undistorter_->undistort(point));
            }

            tempResults.emplace_back(SensingState::NONE, static_cast<unsigned char>(0),
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
            // TODO: Prevent from busy loop. At least, the current maximum fps is 1000.
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

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
