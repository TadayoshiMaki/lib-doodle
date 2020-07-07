#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenManager_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenManager_H_

#include <functional>
#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/DeviceIdGenerator.h"
#include "doodle/IrPen.h"
#include "doodle/IrPenListener.h"
#include "doodle/IrPenOperation.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPenManager : public IrPenStateHandler
{
public:
    static const int kInvalidDeviceId = DeviceIdGenerator::kInvalidId;

    static const Color kInitialRgbLedColor;

    using DevicesUpdatedCallback = std::function<void(const std::vector<IrPenDeviceStatus>)>;
    using StateChangedCallback = std::function<void(const int, const bool)>;
    using ColorReceivedCallback = std::function<void(const int, const Color color)>;
    using RgbLedStateChangedCallback = std::function<void(const int, const RgbLedState)>;
    using RgbLedColorChangedCallback = std::function<void(const int, const Color)>;
    using IrLedStateChangedCallback = std::function<void(const int, const bool)>;

    static std::shared_ptr<IrPenManager> newInstance();

    IrPenManager();
    virtual ~IrPenManager();

    /**
    * @brief Sets the callback to subscribe updates of pen devices.
    * @param owner The owner class of the callable method.
    * @param listenerMethod The callable method with the arguments.
    */
    template<class ListenerClass>
    void setDevicesUpdatedCallback(ListenerClass* owner,
                                   void (ListenerClass::*listenerMethod)(const std::vector<IrPenDeviceStatus>))
    {
        setDevicesUpdatedCallback(std::bind(listenerMethod, owner, std::placeholders::_1));
    }

    /**
    * @brief Sets the callback to subscribe updates of pen devices.
    * @param callback The callable object to subscribe the state.
    */
    virtual void setDevicesUpdatedCallback(const DevicesUpdatedCallback& callback) = 0;

    /**
     * @brief Sets the callback to subscribe the tip switch state of IR pens.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method with the arguments.
     */
    template<class ListenerClass>
    void setStateChangedCallback(ListenerClass* owner,
                                 void (ListenerClass::*listenerMethod)(const int, const bool))
    {
        setStateChangedCallback(std::bind(listenerMethod, owner, std::placeholders::_1, std::placeholders::_2));
    }
    /**
     * @brief Sets the callback to subscribe the tip switch state of IR pens.
     * @param callback The callable object to subscribe the state.
     */
    virtual void setStateChangedCallback(const StateChangedCallback& callback) = 0;

    /**
     * @brief Sets the callback to subscribe the color sensor of IR pens.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method with the arguments.
     */
    template<class ListenerClass>
    void setColorReceivedCallback(ListenerClass* owner,
                                  void (ListenerClass::*listenerMethod)(const int, const Color))
    {
        setColorReceivedCallback(std::bind(listenerMethod,
                                           owner,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
    }

    /**
     * @brief Sets the callback to subscribe the color sensor of IR pens.
     * @param callback The callable object to subscribe the state.
     */
    virtual void setColorReceivedCallback(const ColorReceivedCallback& callback) = 0;

    /**
    * @brief Sets the callback to subscribe the color led state pen devices.
    * @param owner The owner class of the callable method.
    * @param listenerMethod The callable method with the arguments.
    */
    template<class ListenerClass>
    void setRgbLedStateChangedCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const int, const RgbLedState))
    {
        setRgbLedStateChangedCallback(std::bind(listenerMethod,
            owner,
            std::placeholders::_1,
            std::placeholders::_2));
    }

    /**
     * @brief Sets the callback to subscribe the color led state of pen devices.
     * @param callback The callable object to subscribe the state.
     */
    virtual void setRgbLedStateChangedCallback(const RgbLedStateChangedCallback& callback) = 0;

    /**
    * @brief Sets the callback to subscribe the color led of pen devices.
    * @param owner The owner class of the callable method.
    * @param listenerMethod The callable method with the arguments.
    */
    template<class ListenerClass>
    void setRgbLedColorChangedCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const int, const Color))
    {
        setRgbLedColorChangedCallback(std::bind(listenerMethod,
            owner,
            std::placeholders::_1,
            std::placeholders::_2));
    }

    /**
     * @brief Sets the callback to subscribe the color led of pen devices.
     * @param callback The callable object to subscribe the state.
     */
    virtual void setRgbLedColorChangedCallback(const RgbLedColorChangedCallback& callback) = 0;
    
    /**
    * @brief Sets the callback to subscribe the IR led state of pen devices.
    * @param owner The owner class of the callable method.
    * @param listenerMethod The callable method with the arguments.
    */
    template<class ListenerClass>
    void setIrLedStateChangedCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const int, const bool))
    {
        setIrLedStateChangedCallback(std::bind(listenerMethod,
            owner,
            std::placeholders::_1,
            std::placeholders::_2));
    }

    /**
     * @brief Sets the callback to subscribe the IR led state of pen devices.
     * @param callback The callable object to subscribe the state.
     */
    virtual void setIrLedStateChangedCallback(const IrLedStateChangedCallback& callback) = 0;

    /**
     * @brief Initializes or updates the list of pen devices paired with the central device (your computer).
     */
    virtual void initialize() = 0;

    /**
    * @brief Clears all detected pens.
    */
    virtual void finalize() = 0;

    /**
     * @brief Returns the count of pens currently paired.
     */
    virtual size_t getCount() = 0;

    /**
     * @brief Returns the list of pen device IDs including previously paired.
     */
    virtual std::vector<int> getDeviceIds() = 0;

    /**
     * @brief Returns the list of available pen device IDs.
     */
    virtual std::vector<int> getActiveDeviceIds() = 0;

    /**
     * @brief Returns the device ID corresponding to the specified pen.
     * @param pen The pen.
     */
    //virtual int getDeviceId(const std::shared_ptr<IrPen>& pen) = 0;

    /**
     * @brief Returns the pen corresponding to the specified ID.
     * @param id The pen ID.
     */
    virtual std::shared_ptr<IrPen> getPen(const int deviceId) = 0;

    /**
     * @brief Determines whether the pen with the specified ID is connected to the computer.
     * @param deviceId The pen ID.
     */
    virtual bool isConnected(const int deviceId) = 0;

    /**
     * @brief Determines whether the pen is connected to the computer.
     * @param pen The pen.
     */
    //virtual bool isConnected(const std::shared_ptr<IrPen>& pen) = 0;

    /**
     * @brief Determines whether pen devices are detected or not.
     */
    virtual bool empty() = 0;

    /**
     * @brief Returns status of pen devices.
     */
    virtual std::vector<IrPenDeviceStatus> getDeviceStatusList() = 0;

    /**
     * @brief Returns of the specified pen device.
     */
    virtual IrPenDeviceStatus getDeviceStatus(const int deviceId) = 0;

    //IrPen class wrapper for multi irpen thread safe operations
    /**
     * @brief Resumes the pen device. The pen will start to notify of or indicate characteristic value changes.
     */
    virtual bool resume(const int deviceId) = 0;

    /**
     * @brief Pauses the pen device. The pen will stop to notify of or indicate characteristic value changes.
     */
    virtual void pause(const int deviceId) = 0;

    /**
     * @brief Returns the Bluetooth device address.
     */
    virtual std::string getAddress(const int deviceId) = 0;

    /**
     * @brief Sets the IR LED state.
     * @param enable Whether the IR LED is enabled or not.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setIrLedState(const int deviceId, const bool enable) = 0;
    /**
     * @brief Returns the IR LED state.
     * @param enable The obtained value for the IR LED state.
     * @return true if the state is successfully obtained from the device.
     */
    virtual bool getIrLedState(const int deviceId, bool* enable) = 0;

    /**
     * @brief Sets the RGB LED color.
     * @param color The color to set.
     * @return true if the color is successfully set to the device.
     */
    virtual bool setRgbLedColor(const int deviceId, const Color color) = 0;

    /**
     * @brief Sets the RGB LED color.
     * @param r The red value to set.
     * @param g The green value to set.
     * @param b The blue value to set.
     * @return true if the color is successfully set to the device.
     */
    virtual bool setRgbLedColor(const int deviceId, const uint8_t r, const uint8_t g, const uint8_t b) = 0;

    /**
     * @brief Returns the RGB LED color.
     * @param color The obtained color value.
     * @return true if the color is successfully obtained from the device.
     */
    virtual bool getRgbLedColor(const int deviceId, Color* color) = 0;

    /**
     * @brief Sets the RGB LED state.
     * @param state The state to set.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setRgbLedState(const int deviceId, const RgbLedState state) = 0;

    /**
     * @brief Returns the RGB LED state.
     * @param state The obtained value for the RGB LED state.
     * @return true if the state is successfully obtained from the device.
     */
    virtual bool getRgbLedState(const int deviceId, RgbLedState* state) = 0;

    /**
     * @brief Sets the RGB LED state and color.
     * @param state The state to set.
     * @param color The color to set.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setRgbLedState(const int deviceId, const RgbLedState state, const Color color) = 0;

    /**
     * @brief Determines whether the pen tip is pressed or not.
     * @param pressed The tip state.
     * @return true if the state is successfully obtained from the device.
     */
    virtual bool isTipPressed(const int deviceId, bool* pressed) = 0;

    /**
     * @brief Returns the color value which the color sensor is last mesured.
     * @param color The obtained color value.
     */
    virtual bool getColorValue(const int deviceId, Color* color) = 0;

    /**
     * @brief Resets the device state.
     */
    virtual void resetState(const int deviceId) = 0;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenManager_H_
