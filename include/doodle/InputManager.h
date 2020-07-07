#ifndef LIB_DOODLE_FRAMEWORK_doodle_InputManager_H_
#define LIB_DOODLE_FRAMEWORK_doodle_InputManager_H_

#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/HsvSensor.h"
#include "doodle/InputEventDispatcher.h"
#include "doodle/InputEventListener.h"
#include "doodle/InputEventProcessor.h"
#include "doodle/IrPenManager.h"
#include "doodle/MpjScreenSelector.h"
#include "doodle/ScreenManager.h"
#include "doodle/System.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class InputManager
{
public:
    InputManager(const std::shared_ptr<System>& system,
                 const std::shared_ptr<IrPenManager>& penManager,
                 const std::shared_ptr<ScreenManager>& screenManager);
    virtual ~InputManager();

    void addEventListener(const std::shared_ptr<InputEventListener>& l)
    {
        if (eventDispatcher_) eventDispatcher_->addListener(l);
    };

    void removeEventListener(const std::shared_ptr<InputEventListener>& l)
    {
        if (eventDispatcher_) eventDispatcher_->removeListener(l);
    };

    /**
     * @brief Initializes the sensor underlying the input manager.
     *        Note that this takes time to setup devices like a high-speed vision sensor.
     */
    bool initializeSensor();
    /**
     * @brief Finalizes the sensor underlying the input manager.
     */
    void finalizeSensor();
    /**
     * @brief Determines whether the high-speed vision sensor is started or not.
     */
    bool isSensorStarted();
    /**
     * @brief Updates the list of pen devices paired with the computer.
     */
    void updatePenDevices();
    /**
     * @brief Finalizes pen devices.
     */
    void finalizePenDevices();
    /**
     * @brief Returns the list of pen device IDs .
     */
    std::vector<int> getPenDeviceIds();
    ///**
    // * @brief Returns the device ID corresponding to the specified pen.
    // * @param pen The pen.
    // */
    //int getPenDeviceId(const std::shared_ptr<IrPen>& pen);
    /**
     * @brief Returns the count of pens currently paired.
     */
    size_t getPenCount();
    /**
     * @brief Returns the pen corresponding to the specified ID.
     * @param id The pen ID.
     */
    std::shared_ptr<IrPen> getPen(const int deviceId);
    /**
     * @brief Determines whether the specified pen is connected to the computer.
     * @param id The pen ID.
     */
    bool isPenConnected(const int deviceId);
    /**
     * @brief Returns the width of the high-speed vision sensor.
     */
    uint32_t getResolutionWidth();
    /**
     * @brief Returns the height of the high-speed vision sensor.
     */
    uint32_t getResolutionHeight();
    /**
     * @brief Returns the current RGB frame of the high-speed vision sensor.
     */
    std::shared_ptr<cv::Mat> getSensorFrame();
    /**
     * @brief Returns the current detection results.
     */
    std::vector<HsvSensor::SensingResult> getSensingResults();



    /**
     * @brief Invokes a screen initialization event. Should use for debugging.
     * @param e The screen initialization event.
     */
    void invokeScreenInitEvent(const std::shared_ptr<ScreenInitEvent>& e);
    /**
     * @brief Invokes a screen change event. Should use for debugging.
     * @param e The screen change event.
     */
    void invokeScreenChangeEvent(const std::shared_ptr<ScreenChangeEvent>& e);
    /**
     * @brief Invokes an update event for pen devices. Should use for debugging.
     * @param status The device status.
     */
    void invokeDevicesUpdated(const std::vector<IrPenDeviceStatus> status);
    /**
     * @brief Invokes a state change event for the pen device. Should use for debugging.
     * @param deviceId The device ID of a pen.
     * @param pressed Whether the pen tip switch is pressed.
     */
    void invokePenStateChanged(const int deviceId, const bool pressed);
    /**
     * @brief Invokes a color event for the pen device. Should use for debugging.
     * @param deviceId The device ID of a pen.
     * @param color The color in 16bit.
     */
    void invokeColorValueReceived(const int deviceId, const Color color);
    /**
     * @brief Invokes a point detection event. Should use for debugging.
     * @param results The detection results
     */
    void invokeStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results);

private:
    // Doodle system context.
    std::shared_ptr<System> system_;
    // Used manage detected IR pens.
    std::shared_ptr<IrPenManager> penManager_;
    // Used to receive screen events.
    std::shared_ptr<ScreenManager> screenManager_;
    // Used to handle event data and generate events.
    std::shared_ptr<InputEventProcessor> eventProcessor_;
    // Used to dispatch events for IR pens.
    std::shared_ptr<InputEventDispatcher> eventDispatcher_;
    // Used to detect points for pen strokes.
    std::shared_ptr<HsvSensor> strokeDetector_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_InputManager_H_
