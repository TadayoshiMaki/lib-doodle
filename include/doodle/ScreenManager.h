#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenManager_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenManager_H_

#include <memory>
#include <tuple>

#include "mpj_motor.h"

#include "doodle/namespace.h"
#include "doodle/CameraUtils.h"
#include "doodle/MpjScreenLayoutHandler.h"
#include "doodle/MpjScreenLayoutListener.h"
#include "doodle/MpjScreenSelector.h"
#include "doodle/ScreenCoordinateMapper.h"
#include "doodle/ScreenEventDispatcher.h"
#include "doodle/ScreenEventListener.h"
#include "doodle/ScreenEventProcessor.h"
#include "doodle/ScreenProperties.h"
#include "doodle/System.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenManager : public MpjScreenLayoutHandler
{
    using mpj_Angle = mpjsdk::mpj_Angle;
    using mpj_Camera = mpjsdk::mpj_Camera;
    using mpj_Image = mpjsdk::mpj_Image;
    using mpj_MotorManager = mpjsdk::mpj_MotorManager;

public:
    static const int kInvalidScreenId = MpjScreenSelector::kInvalidScreenId;

    ScreenManager(const std::shared_ptr<System>& system);
    virtual ~ScreenManager();

    void addEventListener(const std::shared_ptr<ScreenEventListener>& l)
    {
        if (eventDispatcher_) eventDispatcher_->addListener(l);
    };

    void removeEventListener(const std::shared_ptr<ScreenEventListener>& l)
    {
        if (eventDispatcher_) eventDispatcher_->removeListener(l);
    };

    /**
     * @bfief Initializes the screen control devices and loads screen layout file.
     */
    void initializeDevices();
    /**
     * @brief Finalizes the screen control devices.
     */
    void finalizeDevices();
    /**
     * @bfief Starts the auto screen control.
     */
    void startAutoScreenControl();
    /**
     * @bfief Stops the auto screen control.
     */
    void stopAutoScreenControl();
    /**
     * @brief Determines whether the auto screen control is started.
     */
    bool isAutoScreenControlStarted();
    /**
     * @brief Returns the current motors angle.
     */
    mpj_Angle getCurrentAngle();
    /**
     * @brief Sets the current screen if a screen contains them on an image of the fisheye camera.
     * @param x The x-axis value on an image.
     * @param y The y-axis value on an image.
     */
    void changeScreenIfNeeded(const int x, const int y);
    /**
     * @brief Returns the fisheye camera.
     */
    std::shared_ptr<mpj_Camera> getFisheyeCamera();
    /**
     * @brief Returns the motor manager to move the angle of the projector.
     */
    std::shared_ptr<mpj_MotorManager> getMotorManager();
    /**
     * @brief Determines whether the current screen is being changed.
     */
    bool isScreenChanging();
    /**
     * @brief Returns the current frame image on the fisheye camera.
     */
    std::shared_ptr<const mpj_Image> getFisheyeCameraFrame();
    /**
     * @brief Returns screens registered to the manager.
     */
    std::vector<std::shared_ptr<Screen>> getScreens();
    /**
     * @brief Returns the screen for the specified ID. A screen includes data
     *        which describes a position on sensor views.
     * @param screenId The screen ID.
     */
    std::shared_ptr<Screen> getScreen(const int screenId);

    /**
     * @brief Sets the current screen by using the ID.
     * @param screenId The screen ID.
     */
    void setCurrentScreen(const int screenId);
    /**
     * @brief Returns the current screen ID. -1 if no current screen.
     */
    int getCurrentScreenId();
    /**
     * @brief Returns the coordinate mapper for the screen which has the specified ID.
     * @param screenId The screen ID.
     */
    std::shared_ptr<ScreenCoordinateMapper> getCoordinateMapper(const int screenId);
    /**
     * @brief Returns the current coordinate mapper for the screen which has the specified ID.
     */
    std::shared_ptr<ScreenCoordinateMapper> getCurrentCoordinateMapper();

    /**
     * @brief Returns screen properties registered to the manager. A property includes a size of a screen.
     */
    std::vector<std::shared_ptr<ScreenProperty>> getScreenProperties();
    /**
     * @brief Returns the screen property.
     * @param screenId The screen ID.
     */
    std::shared_ptr<ScreenProperty> getScreenProperty(const int screenId);

private:
    // Empty screen layouts.
    const std::vector<std::shared_ptr<Screen>> kEmptyLayouts;

    // Doodle system environment.
    std::shared_ptr<System> system_;
    // uEye camera instance.
    // TODO: Wrap a class like FisheyeCamera.
    std::shared_ptr<mpj_Camera> fisheyeCamera_;
    // Used to get the motor state.
    std::shared_ptr<mpj_MotorManager> motorManager_;
    // Used to move the projector between predefined screens.
    std::shared_ptr<MpjScreenSelector> screenSelector_;
    // Used to map coordinates to screen coordinates.
    std::map<int, std::shared_ptr<ScreenCoordinateMapper>> mappers_;
    // Used to undistort screen coordinates.
    std::shared_ptr<Undistorter> undistorter_;
    // The properties of current screens.
    std::map<int, std::shared_ptr<ScreenProperty>> screenPropertyEntries_;
    // The current screen mapper.
    std::shared_ptr<ScreenCoordinateMapper> currentMapper_;
    // The callback interface to notify of the screens state.
    std::vector<std::shared_ptr<ScreenEventListener>> listeners_;
    // Used to process screen events.
    std::shared_ptr<ScreenEventProcessor> eventProcessor_;
    // Used to dispatch events for screens.
    std::shared_ptr<ScreenEventDispatcher> eventDispatcher_;

    /**
     * @brief Initializes motors underlying the screen manager with the specified parameters.
     * @param model The model name of motors.
     * @param comPort The string representation of a COM port number.
     * @param paramsFile The motor parameter filename.
     */
    void initializeMotors(const char* model,
                          const char* comPort,
                          const std::string& paramsFilePath);
    /**
     * @brief Finalizes the motors.
     */
    void finalizeMotors();
    /**
     * @brief Initializes a fisheye camera underlying the screen manager.
     * @param model The model name of a fisheye camera.
     */
    void initializeFisheyeCamera(const std::string& model);
    /**
     * @brief Finalizes the fisheye camera.
     */
    void finalizeFisheyeCamera();
    /**
     * @brief Initializes a screen selector underlying the screen manager with the screen configuration file.
     * @param screenLayoutFilename The JSON screen data file path.
     * @param maskFilePath The mask used at the point detection for screens.
     */
    void initializeScreenSelector(const std::string& screenLayoutFilename,
                                  const std::string& maskFilePath = std::string());
    /**
     * @brief Finalizes the screen selector.
     */
    void finalizeScreenSelector();



    /**
     * @brief Called when screen layouts are loaded.
     * @param layouts The screen layouts loaded.
     */
    virtual void onScreensLoaded(const std::vector<std::shared_ptr<Screen>>& screens) override;

    /**
     * @brief Called when the screen change is started.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    virtual void onScreenChangeStarted(const int oldScreenId, const int newScreenId) override;

    /**
     * @brief Called when the current screen is changed.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    virtual void onCurrentScreenChanged(const int oldScreenId, const int newScreenId) override;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenManager_H_
