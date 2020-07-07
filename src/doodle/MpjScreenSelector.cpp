#include "doodle/MpjScreenSelector.h"

#include <filesystem>
#include <fstream>

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

MpjScreenSelector::MpjScreenSelector(const std::shared_ptr<mpj_Camera>& camera,
                                     const std::shared_ptr<mpj_MotorManager>& motorManager) :
    camera_(camera),
    motorManager_(motorManager),
    currentScreenId_(kInvalidScreenId),
    motorMoving_(false)
{
    if (camera_)
    {
        // TODO: Should load camera parameters from a setting file.
        pointDetector_ = std::make_shared<AsyncPointDetector>(camera_);
    }

}

MpjScreenSelector::~MpjScreenSelector()
{
    for (auto&& l : listeners_)
    {
        l.reset();
    }
    listeners_.clear();
}

bool MpjScreenSelector::loadScreens(const std::string& dataFilePath)
{
    auto loader = std::make_shared<JsonScreensConfigLoader>();
    auto screensConfig = loader->load(dataFilePath);
    if (!screensConfig) return false;

    auto& screens = screensConfig->getScreens();
    screensEntries_.clear();
    screenChangers_.clear();
    for (auto&& screen : screens)
    {
        auto screenId = screen->getId();
        screensEntries_.emplace(std::piecewise_construct,
                                std::forward_as_tuple(screenId),
                                std::forward_as_tuple(std::move(screen)));

        auto screenChanger = std::make_shared<ScreenChangeTrigger>(screenId,
                                                                   kScreenChangeThreshold,
                                                                   this);
        screenChangers_.push_back(std::move(screenChanger));
    }

    dispatchScreensInitialized(getScreens());
    return true;
}

void MpjScreenSelector::start()
{
    if (!pointDetector_) return;

    pointDetector_->start(
        [this](std::vector<mpj_DetectedPoint> points)
    {
        for (auto&& point : points)
        {
            // TODO: A latter point currently has priority.
            changeScreenIfNeeded(point.x, point.y);
        }
    }, [this](const std::shared_ptr<const mpj_Image>& frame)
    {
        currentImage_ = frame;
    });
}

void MpjScreenSelector::stop()
{
    if (!pointDetector_) return;
    pointDetector_->stop();
}

bool MpjScreenSelector::isStarted()
{
    if (!pointDetector_) return false;
    return pointDetector_->isStarted();
}

bool MpjScreenSelector::isMotorMoving()
{
    return motorMoving_;
}

std::shared_ptr<const mpj_Image> MpjScreenSelector::getCurrentFrame()
{
    return currentImage_;
}

std::vector<std::shared_ptr<Screen>> MpjScreenSelector::getScreens()
{
    std::vector<std::shared_ptr<Screen>> screens;
    for (auto&& screenEntry : screensEntries_)
    {
        screens.push_back(screenEntry.second);
    }
    return std::move(screens);
}

std::shared_ptr<Screen> MpjScreenSelector::getScreen(const int screenId)
{
    return screensEntries_.at(screenId);
}

void MpjScreenSelector::changeScreenIfNeeded(const Point p)
{
    changeScreenIfNeeded(p.x, p.y);
}

void MpjScreenSelector::changeScreenIfNeeded(const int x, const int y)
{
    if (screensEntries_.empty()) return;

    if (isMotorMoving()) return;

    int screenId = kInvalidScreenId;
    // Note that the result depends on data structure to hold screen information.
    for (auto&& screenEntry : screensEntries_)
    {
        if (screenEntry.second->contains(x, y))
        {
            screenId = screenEntry.second->getId();
            break;
        }
    }

    if (screenId == kInvalidScreenId || screenId == currentScreenId_) return;

    for (auto&& screenChanger : screenChangers_)
    {
        if (screenChanger->targetScreenEquals(screenId))
        {
            screenChanger->countDown();
        }
        else
        {
            screenChanger->reset();
        }
    }
}

void MpjScreenSelector::setCurrentScreen(const int screenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenId < 0) return;
    // No need to handle same screen ID.
    if (currentScreenId_ == screenId) return;

    auto it = screensEntries_.find(screenId);
    if (it == screensEntries_.end()) return;

    const int oldScreenId = currentScreenId_;
    const int newScreenId = it->second->getId();

    dispatchScreenChangeStarted(oldScreenId, newScreenId);
    if (motorManager_)
    {
        motorMoving_ = true;
        if (shouldMoveMotors(it->second->getAngle()))
        {
            motorManager_->setGoalAngle(it->second->getAngle());
        }
    }
    currentScreenId_ = screenId;
    if (motorManager_)
    {
        // Prevent from accessing the motor status while moving.
        while (motorManager_->isMotorRotating()) continue;
        motorMoving_ = false;
    }
    dispatchCurrentScreenChanged(oldScreenId, newScreenId);
    resetScreenChangeTriggers();
}

int MpjScreenSelector::getCurrentScreenId()
{
    return currentScreenId_;
}

void MpjScreenSelector::setMask(const std::string& maskFilePath)
{
    if (pointDetector_) pointDetector_->setMask(maskFilePath);
}

void MpjScreenSelector::setCameraNoiseBrightness(const float value)
{
    if (pointDetector_) pointDetector_->setZeta(value);
}

bool MpjScreenSelector::shouldMoveMotors(const Angle& goalAngle)
{
    auto params = motorManager_->getMotorParamsLastApplied();
    auto currentAngle = motorManager_->getCurrentAngle();
    float d = goalAngle.distance(currentAngle);
    return (d > params.convergence);
}

void MpjScreenSelector::resetScreenChangeTriggers()
{
    for (auto&& screenChanger : screenChangers_)
    {
        screenChanger->reset();
    }
}

void MpjScreenSelector::dispatchScreensInitialized(const std::vector<std::shared_ptr<Screen>>& screens)
{
    for (auto&& listener : listeners_)
    {
        listener->onScreensLoaded(screens);
    }
}

void MpjScreenSelector::dispatchScreenChangeStarted(const int oldScreenId, const int newScreenId)
{
    for (auto&& listener : listeners_)
    {
        listener->onScreenChangeStarted(oldScreenId, newScreenId);
    }
}

void MpjScreenSelector::dispatchCurrentScreenChanged(const int oldScreenId, const int newScreenId)
{
    for (auto&& listener : listeners_)
    {
        listener->onCurrentScreenChanged(oldScreenId, newScreenId);
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
