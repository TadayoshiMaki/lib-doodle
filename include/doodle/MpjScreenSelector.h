#ifndef LIB_DOODLE_FRAMEWORK_doodle_MpjScreenSelector_H_
#define LIB_DOODLE_FRAMEWORK_doodle_MpjScreenSelector_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "mpj_angle.h"
#include "mpj_camera.h"
#include "mpj_motor.h"

#include "doodle/namespace.h"
#include "doodle/AsyncPointDetector.h"
#include "doodle/MpjScreenLayoutListener.h"
#include "doodle/ScreensConfig.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using mpjsdk::mpj_Camera;
using mpjsdk::mpj_Image;
using mpjsdk::mpj_MotorManager;

class ScreenChangeTrigger;

class MpjScreenSelector
{
public:
    static const int kInvalidScreenId = -1;

    MpjScreenSelector(const std::shared_ptr<mpj_Camera>& camera,
                      const std::shared_ptr<mpj_MotorManager>& motorManager);
    virtual ~MpjScreenSelector();

    void addListener(const std::shared_ptr<MpjScreenLayoutListener>& l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const std::shared_ptr<MpjScreenLayoutListener>& l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it != listeners_.end())
        {
            listeners_.erase(it);
        }
    };

    /**
     * @brief Loads screen data from the specified data file path.
     * @param[in] screenDataPath The JSON data file path.
     * @return true if successfully loaded.
     */
    bool loadScreens(const std::string& dataFilePath);

    /**
     * @brief Starts the screen selector.
     */
    void start();

    /**
     * @brief Stops the screen selector.
     */
    void stop();

    /**
     * @brief Determines whether the screen selector is started.
     * @return true if the screen selector is started.
     */
    bool isStarted();

    /**
     * @brief Determines whether at least an one motor is moving for a screen selection.
     */
    bool isMotorMoving();

    /**
     * @brief Returns the current frame to be processed with the point detection.
     * @return The current frame to be processed.
     */
    std::shared_ptr<const mpj_Image> getCurrentFrame();

    /**
     * @brief Returns screens which are loaded from a file.
     * @return The screens including their contour.
     */
    std::vector<std::shared_ptr<Screen>> getScreens();

    /**
     * @brief Returns the screen for the specified ID.
     * @param screenId The screen ID.
     */
    std::shared_ptr<Screen> getScreen(const int screenId);

    /**
     * @brief Change the current screen with a contour which contains the specified coordicates
     *        in an image captured by a fisheye lens camara. Note that this method blocks
     *        your thread until completed.
     * @param p The detected point on a fisheye lens camera view.
     */
    void changeScreenIfNeeded(const Point p);

    /**
     * @brief Change the current screen with a contour which contains the specified coordicates
     *        in an image captured by a fisheye camara. Note that this method blocks your thread
     *        until completed.
     * @param x The x-coordicate.
     * @param y The y-coordinate.
     */
    void changeScreenIfNeeded(const int x, const int y);

    /**
     * @brief Sets the screen with the specified ID. Note that this method blocks your thread until completed.
     * @param screenId The valid screen ID.
     */
    void setCurrentScreen(const int screenId);

    /**
     * @brief Returns the current screen ID. -1 if no current screen.
     */
    int getCurrentScreenId();

    /**
     * @brief Sets the mask for point detection.
     * @param maskFilePath The path of a mask file.
     */
    void setMask(const std::string& maskFilePath);

    /**
     * @brief Sets the brightness value of noises including salt-and-pepper noise
     *        which depends on only the gain parameter of a camera.
     * @param value The brightness value of noises.
     */
    void setCameraNoiseBrightness(const float value);

private:
    // The detection count until screen changes.
#ifndef _DEBUG
    static constexpr int kScreenChangeThreshold = 5;
#else
    static constexpr int kScreenChangeThreshold = 1;
#endif

    // The motor to move the projector.
    std::shared_ptr<mpj_MotorManager> motorManager_;
    // The lock object for moving to a screen.
    std::mutex mutex_;
    // The camera used to capture frames.
    std::shared_ptr<mpj_Camera> camera_;
    // The asynchronous point detector.
    std::shared_ptr<AsyncPointDetector> pointDetector_;
    // The current frame to be processed with the point detection.
    std::shared_ptr<const mpj_Image> currentImage_;

    // The map of screen layouts.
    std::map<int, std::shared_ptr<Screen>> screensEntries_;
    // Used to change the current screen if conditions are met.
    std::vector<std::shared_ptr<ScreenChangeTrigger>> screenChangers_;

    // The callback interface to notify of screen state changes.
    std::vector<std::shared_ptr<MpjScreenLayoutListener>> listeners_;

    // The current screen ID.
    int currentScreenId_;
    // Determines whether a motor is moving.
    std::atomic<bool> motorMoving_;

    /**
     * @brief Determines whether the motors should be moved to the specified goal angle.
     * @param goalAngle The goal angle.
     */
    bool shouldMoveMotors(const Angle& goalAngle);

    /**
     * @brief Resets the state of screen change triggers.
     */
    void resetScreenChangeTriggers();

    /**
     * @brief Notifies of when screen layouts are initialized.
     * @param layouts The initialized layouts.
     */
    void dispatchScreensInitialized(const std::vector<std::shared_ptr<Screen>>& screens);
    /**
     * @brief Notifies of when the screen change is started.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    void dispatchScreenChangeStarted(const int oldScreenId, const int newScreenId);

    /**
     * @brief Notifies of when the current screen is changed.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    void dispatchCurrentScreenChanged(const int oldScreenId, const int newScreenId);
};



class ScreenChangeTrigger
{
public:
    ScreenChangeTrigger(const int screenId,
        const int count,
        MpjScreenSelector* selector) :
        screenId_(screenId),
        count_(count),
        currentCount_(count),
        selector_(selector)
    {
    }

    virtual ~ScreenChangeTrigger()
    {
    }

    int getTargetScreenId()
    {
        return screenId_;
    }

    bool targetScreenEquals(const int screenId)
    {
        return (screenId_ == screenId);
    }

    int getCount()
    {
        return currentCount_;
    }

    /**
     * @brief Decrements the count, the callback will run if the count reathes zero.
     */
    void countDown()
    {
        currentCount_--;
        if (currentCount_ < 0)
        {
            selector_->setCurrentScreen(screenId_);
            reset();
        }
    }

    /**
     * @brief Resets the count.
     */
    void reset()
    {
        currentCount_ = count_;
    }

private:
    // The target screen ID.
    const int screenId_;
    // The count of this trigger.
    const int count_;
    // The current count.
    int currentCount_;
    // The screen selector.
    MpjScreenSelector* selector_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_MpjScreenSelector_H_
