#include "doodle/ScreenManager.h"

#include "mpj_motor.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

ScreenManager::ScreenManager(const std::shared_ptr<System>& system) :
    system_(system)
{
    eventDispatcher_ = std::make_shared<ScreenEventDispatcher>();
    eventProcessor_ = std::make_shared<ScreenEventProcessor>(eventDispatcher_);
    eventProcessor_->start();

    auto loader = std::make_shared<JsonCameraParamsLoader>();
    auto& sensorProperties = system_->getVisionSensorProperties();
    auto& calibFilePath = sensorProperties->getCalibrationFilePath();
    auto cameraParams = loader->load(calibFilePath);
    undistorter_ = std::make_shared<Undistorter>(cameraParams);
}

ScreenManager::~ScreenManager()
{
    if (motorManager_) motorManager_.reset();
    if (fisheyeCamera_) fisheyeCamera_.reset();
    if (screenSelector_)
    {
        if (screenSelector_->isStarted())
        {
            screenSelector_->stop();
        }
        screenSelector_->removeListener(shared_from_this());
        screenSelector_.reset();
    }

    mappers_.clear();

    eventDispatcher_.reset();
    if (eventProcessor_)
    {
        eventProcessor_->stop();
        eventProcessor_.reset();
    }
}

void ScreenManager::initializeDevices()
{
    auto& motorProperties = system_->getMotorProperties();
    if (motorProperties)
    {
        auto& model = motorProperties->getModel();
        auto& comPort = motorProperties->getComPort();
        auto& paramsFilename = motorProperties->getParameterFilePath();
        initializeMotors(model.c_str(), comPort.c_str(), paramsFilename);
    }


    auto& cameraProperties = system_->getFisheyeCameraProperties();
    std::string maskFilePath;
    if (cameraProperties)
    {
        initializeFisheyeCamera(cameraProperties->getModel());
        maskFilePath = cameraProperties->getMaskFilePath();
    }


    auto& layoutProperties = system_->getScreenLayoutProperties();
    if (layoutProperties)
    {
        initializeScreenSelector(layoutProperties->getFilePath(), maskFilePath);
    }
}

void ScreenManager::finalizeDevices()
{
    finalizeScreenSelector();
    finalizeMotors();
    finalizeFisheyeCamera();
}

void ScreenManager::startAutoScreenControl()
{
    if (!screenSelector_) return;
    screenSelector_->start();
}

void ScreenManager::stopAutoScreenControl()
{
    if (!screenSelector_) return;
    screenSelector_->stop();
}

bool ScreenManager::isAutoScreenControlStarted()
{
    if (!screenSelector_) return false;
    return screenSelector_->isStarted();
}

mpj_Angle ScreenManager::getCurrentAngle()
{
    if (!motorManager_) return mpj_Angle(0, 0);
    return motorManager_->getCurrentAngle();
}

void ScreenManager::changeScreenIfNeeded(const int x, const int y)
{
    if (screenSelector_) screenSelector_->changeScreenIfNeeded(x, y);
}

std::shared_ptr<mpj_Camera> ScreenManager::getFisheyeCamera()
{
    return fisheyeCamera_;
}

std::shared_ptr<mpj_MotorManager> ScreenManager::getMotorManager()
{
    return motorManager_;
}

bool ScreenManager::isScreenChanging()
{
    if (!screenSelector_) return false;
    return screenSelector_->isMotorMoving();
}

std::shared_ptr<const mpj_Image> ScreenManager::getFisheyeCameraFrame()
{
    if (!screenSelector_) return std::shared_ptr<const mpj_Image>();
    return screenSelector_->getCurrentFrame();
}

std::vector<std::shared_ptr<Screen>> ScreenManager::getScreens()
{
    if (!screenSelector_) return kEmptyLayouts;
    return screenSelector_->getScreens();
}

std::shared_ptr<Screen> ScreenManager::getScreen(const int screenId)
{
    if (!screenSelector_) return nullptr;
    return screenSelector_->getScreen(screenId);
}

void ScreenManager::setCurrentScreen(const int screenId)
{
    if (screenSelector_) screenSelector_->setCurrentScreen(screenId);
}

int ScreenManager::getCurrentScreenId()
{
    if (!screenSelector_) return MpjScreenSelector::kInvalidScreenId;
    return screenSelector_->getCurrentScreenId();
}

std::shared_ptr<ScreenCoordinateMapper> ScreenManager::getCoordinateMapper(const int screenId)
{
    auto& it = mappers_.find(screenId);
    if (it != mappers_.end()) return it->second;

    return nullptr;
}

std::shared_ptr<ScreenCoordinateMapper> ScreenManager::getCurrentCoordinateMapper()
{
    int currentScreenId = getCurrentScreenId();
    return getCoordinateMapper(currentScreenId);
}

std::vector<std::shared_ptr<ScreenProperty>> ScreenManager::getScreenProperties()
{
    std::vector<std::shared_ptr<ScreenProperty>> properties;
    for (auto&& screenProperty : screenPropertyEntries_)
    {
        properties.push_back(screenProperty.second);
    }
    return std::move(properties);
}

std::shared_ptr<ScreenProperty> ScreenManager::getScreenProperty(const int screenId)
{
    auto& it = screenPropertyEntries_.find(screenId);
    if (it != screenPropertyEntries_.end()) return it->second;

    return nullptr;
}

void ScreenManager::initializeMotors(const char* model,
                                     const char* comPort,
                                     const std::string& paramsFilePath)
{
    using mpjsdk::mpj_MotorParams;
    using mpjsdk::mpj_IoException;

    mpj_MotorParams params;
    _FSPFX path jsonFileName(paramsFilePath);
    if (_FSPFX exists(jsonFileName))
    {
    }
    else
    {
    }

    try
    {
    }
    catch (const mpj_IoException& e)
    {
    }
}

void ScreenManager::finalizeMotors()
{
    if (motorManager_) motorManager_.reset();
}

void ScreenManager::initializeFisheyeCamera(const std::string& model)
{
    using mpjsdk::mpj_FunctionFailureException;
    using mpjsdk::mpj_CameraConfigManager;

    try
    {
    }
    catch (mpj_FunctionFailureException e)
    {
        // uEye camera not found.
    }

    if (!fisheyeCamera_) return;

    auto config = fisheyeCamera_->getConfigManager();
    auto mask = config->getSupportedConfigs();
    if (mpj_CameraConfigManager::c_Gain & mask)
    {
        // TODO: Should load a value from a file.
        config->setGain(100.f);
    }
    if (mpj_CameraConfigManager::c_Exposure & mask)
    {
        // TODO: Should load a value from a file.
        config->setExposure(30.f);
    }
    if (mpj_CameraConfigManager::c_PixelClock & mask)
    {
        // TODO: Should load a value from a file.
        config->setPixelClock(30.f);
    }
}

void ScreenManager::finalizeFisheyeCamera()
{
    if (fisheyeCamera_) fisheyeCamera_.reset();
}

void ScreenManager::initializeScreenSelector(const std::string& screenConfigFilePath,
                                             const std::string& maskFilePath)
{
    screenSelector_ = std::make_shared<MpjScreenSelector>(fisheyeCamera_, motorManager_);
    if (!maskFilePath.empty())
    {
        screenSelector_->setMask(maskFilePath);
    }
    screenSelector_->addListener(shared_from_this());
    screenSelector_->loadScreens(screenConfigFilePath);
}

void ScreenManager::finalizeScreenSelector()
{
    if (screenSelector_->isStarted())
    {
        screenSelector_->stop();
    }
    screenSelector_->removeListener(shared_from_this());
    screenSelector_.reset();
}

void ScreenManager::onScreensLoaded(const std::vector<std::shared_ptr<Screen>>& screens)
{
    // Refresh screen coordinate mappers.
    mappers_.clear();
    auto& projectorProperties = system_->getProjectorProperties();
    System::Resolution resolution = projectorProperties->getResolution();
    // Used to notify of screen initialization.
    std::vector<std::shared_ptr<ScreenProperty>> screenProperties;
    for (auto&& screen : screens)
    {
        int id = screen->getId();

        // Undistort points before calculating a homography matrix.
        std::vector<PointF> srcPoints;
        auto& points = screen->getPlanePoints();
        for (auto&& point : points)
        {
            if (undistorter_)
            {
                auto& srcPoint = undistorter_->undistort(point);
                srcPoints.emplace_back(srcPoint->x, srcPoint->y);
            }
            else
            {
                srcPoints.emplace_back(point.x, point.y);
            }
        }

        std::vector<PointF> dstPoints;
        auto& refPoints = screen->getPlaneReferencePoints();
        for (auto&& refPoint : refPoints)
        {
            dstPoints.emplace_back(refPoint.x, refPoint.y);
        }
        auto mapper = std::make_shared<ScreenCoordinateMapper>(srcPoints, dstPoints);
        mappers_.emplace(std::piecewise_construct,
                         std::forward_as_tuple(id),
                         std::forward_as_tuple(mapper));

        auto screenProperty = std::make_shared<ScreenProperty>(id,
                                                               resolution.width,
                                                               resolution.height);
        screenPropertyEntries_.emplace(std::piecewise_construct,
                                       std::forward_as_tuple(id),
                                       std::forward_as_tuple(screenProperty));

        screenProperties.push_back(screenProperty);
    }

    if (eventProcessor_) eventProcessor_->handleScreensInitialized(screenProperties);
}

void ScreenManager::onScreenChangeStarted(const int oldScreenId, const int newScreenId)
{
    if (eventProcessor_) eventProcessor_->handleScreenChangeStarted(oldScreenId, newScreenId);
}

void ScreenManager::onCurrentScreenChanged(const int oldScreenId, const int newScreenId)
{
    auto& it = mappers_.find(newScreenId);
    if (it != mappers_.end())
    {
        currentMapper_ = it->second;
    }

    if (eventProcessor_) eventProcessor_->handleCurrentScreenChanged(oldScreenId, newScreenId);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
