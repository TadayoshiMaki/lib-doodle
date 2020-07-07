#include "doodle/InputManager.h"

#include "mpj_angle.h"
#include "mpj_motor.h"

#include "doodle/CameraUtils.h"
#include "HsvSensorProdMultitrack.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

InputManager::InputManager(const std::shared_ptr<System>& system,
                           const std::shared_ptr<IrPenManager>& penManager,
                           const std::shared_ptr<ScreenManager>& screenManager) :
    system_(system), penManager_(penManager), screenManager_(screenManager)
{
    auto& properties = system_->getVisionSensorProperties();
    auto loader = std::make_shared<JsonCameraParamsLoader>();
    auto& calibFilePath = properties->getCalibrationFilePath();
    auto cameraParams = loader->load(calibFilePath);
    auto undistorter = std::make_shared<Undistorter>(cameraParams);

    strokeDetector_ = HsvSensorFactory::create(properties->getModel());
    strokeDetector_->setUndistorter(undistorter);
    strokeDetector_->setFps(500);
    strokeDetector_->setCaptureEnabled(false);

    eventDispatcher_ = std::make_shared<InputEventDispatcher>();
    eventProcessor_ = std::make_shared<InputEventProcessor>(penManager_, screenManager_, eventDispatcher_, strokeDetector_);
    screenManager_->addEventListener(eventProcessor_);
    eventProcessor_->start();
}

InputManager::~InputManager()
{
    strokeDetector_.reset();

    screenManager_->removeEventListener(eventProcessor_);
    eventProcessor_->stop();
    eventProcessor_.reset();
    eventDispatcher_.reset();

    penManager_.reset();

    screenManager_.reset();
}

bool InputManager::initializeSensor()
{
    auto& properties = system_->getVisionSensorProperties();
    // Note that it takes time to apply parameters.
    auto& config = properties->getConfiguration();
    if (!strokeDetector_->open(config))
    {
        // Failed to open the sensor.
        return false;
    }
#ifdef DEBUG_CAMIMAGE_
    auto a = std::dynamic_pointer_cast<SentechHsvSensorMultiTrack>(strokeDetector_);
    a->setCaptureEnabled(true);
#endif
    return true;
}

void InputManager::finalizeSensor()
{
    if (strokeDetector_->isStarted()) strokeDetector_->stop();
    if (strokeDetector_->isOpened()) strokeDetector_->close();
}

bool InputManager::isSensorStarted()
{
    return strokeDetector_->isStarted();
}

void InputManager::updatePenDevices()
{
    penManager_->initialize();
}

void InputManager::finalizePenDevices()
{
    penManager_->finalize();
}

std::vector<int> InputManager::getPenDeviceIds()
{
    return penManager_->getDeviceIds();
}

//int InputManager::getPenDeviceId(const std::shared_ptr<IrPen>& pen)
//{
//    return penManager_->getDeviceId(pen);
//}

size_t InputManager::getPenCount()
{
    return penManager_->getCount();
}

std::shared_ptr<IrPen> InputManager::getPen(const int deviceId)
{
    return penManager_->getPen(deviceId);
}

bool InputManager::isPenConnected(const int deviceId)
{
    return penManager_->isConnected(deviceId);
}

uint32_t InputManager::getResolutionWidth()
{
    return strokeDetector_->getWidth();
}

uint32_t InputManager::getResolutionHeight()
{
    return strokeDetector_->getHeight();
}

std::shared_ptr<cv::Mat> InputManager::getSensorFrame()
{
    return strokeDetector_->getCurrentFrame();
}

std::vector<HsvSensor::SensingResult> InputManager::getSensingResults()
{
    return strokeDetector_->getCurrentSensingResults();
}

void InputManager::invokeScreenInitEvent(const std::shared_ptr<ScreenInitEvent>& e)
{
    eventProcessor_->onScreenInitEvent(e);
}

void InputManager::invokeScreenChangeEvent(const std::shared_ptr<ScreenChangeEvent>& e)
{
    eventProcessor_->onScreenChangeEvent(e);
}

void InputManager::invokeDevicesUpdated(const std::vector<IrPenDeviceStatus> status)
{
    eventProcessor_->onDevicesUpdated(status);
}

void InputManager::invokePenStateChanged(const int deviceId, const bool pressed)
{
    eventProcessor_->onPenStateChanged(deviceId, pressed);
}

void InputManager::invokeColorValueReceived(const int deviceId, const Color color)
{
    eventProcessor_->onColorValueReceived(deviceId, color);
}

void InputManager::invokeStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results)
{
    eventProcessor_->onStrokePointDetected(results);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
