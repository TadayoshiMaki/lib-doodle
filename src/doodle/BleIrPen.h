#ifndef LIB_DOODLE_FRAMEWORK_doodle_BleIrPen_H_
#define LIB_DOODLE_FRAMEWORK_doodle_BleIrPen_H_

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <utility>
#include <string>

#include "ble_device.h"
#include "ble_gatt_characteristic.h"
#include "ble_gatt_service.h"
#include "doodle/namespace.h"
#include "doodle/IrPen.h"
#include "doodle/Ticker.h"
#include "doodle/MemoryUtils.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using namespace ble;

class BleIrPen final :
    public IrPen,
    public BleGattCharacteristicListener,
    public inheritable_enable_shared_from_this<BleIrPen>
{
    using BleGattCharacteristicMap = std::map<std::string, std::shared_ptr<BleGattCharacteristic>>;
    using BleGattCharacteristicEntry = std::pair<std::string, std::shared_ptr<BleGattCharacteristic>>;
    friend class BleIrPenManager;

public:
    BleIrPen(const std::shared_ptr<BleDevice>& device);
    virtual ~BleIrPen();

    virtual bool resume() override;
    virtual void pause() override;
    virtual bool isResumed() override;

    /**
     * @brief Returns the Bluetooth device address.
     */
    std::string getAddress();

    /**
     * @brief Returns the BLE device underlying the IR pen.
     */
    std::shared_ptr<BleDevice> getDevice();

    /**
     * @brief Determines whether the device is connected to the computer.
     */
    bool isConnected();

    /**
     * @brief Returns the characteristic corresponding to the specified UUID.
     */
    std::shared_ptr<BleGattCharacteristic> getCharacteristic(const std::string& uuid);

    /**
     * @brief Resets the device state.
     */
    void resetState();

protected:
    /**
     * @brief Subscribes server messages (notification and indication).
     * @return true if subscription is successfully started.
     */
    virtual bool subscribeServerMessages() override;
    /**
     * @brief Unsubscribes server messages (notification and indication).
     */
    virtual void unsubscribeServerMessages() override;

    /**
     * @brief Called when the pen device is resumed.
     */
    virtual void onResume() override;

    /**
     * @brief Called when the pen device is paused.
     */
    virtual void onPause() override;

    /**
     * @brief Dispatches resume state notification of the pen device.
     */
    virtual void dispatchResume() override;

    /**
     * @brief Dispatches pause state notification of the pen device.
     */
    virtual void dispatchPause() override;

private:
    // The time interval to attempt to reconnect.
    static constexpr int kAutoReconnectIntervalMillis = 5000;

    // Whether this pen is resumed or not.
    std::atomic_bool resumed_;

    // The lock object for pen status.
    std::mutex mutex_;

    // The pen device instance.
    std::shared_ptr<BleDevice> device_;

    // The GATT service.
    std::shared_ptr<BleGattService> service_;

    // The pairs of UUID and characteristic.
    BleGattCharacteristicMap characteristicMap_;

    // Used to attempt to reconnect periodically.
    std::unique_ptr<Ticker> reconnector_;

    // The lock object for reconnection.
    std::mutex reconnectionMutex_;

    // Whether auto reconnection is enabled or not.
    std::atomic_bool autoReconnectionEnabled_;

    /**
     * @brief Called to attempt to reconnect.
     */
    void onReconnect();

    /**
     * @brief Dispatches reconnection notification of the pen device.
     */
    void dispatchReconnect();

    /**
     * @brief Initializes characteristics of the service.
     */
    void initCharacteristics();
    /**
     * @brief Sets the IR LED state.
     * @param enable Whether the IR LED is enabled or not.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setIrLedState(const bool enable);
    /**
     * @brief Returns the IR LED state.
     * @param enable The obtained value for the IR LED state.
     * @return true if the state is successfully obtained from the device.
     */
    bool getIrLedState(bool* enable);
    /**
     * @brief Sets the RGB LED color.
     * @param color The color to set.
     * @return true if the color is successfully set to the device.
     */
    virtual bool setRgbLedColor(const Color color);
    /**
     * @brief Sets the RGB LED color.
     * @param r The red value to set.
     * @param g The green value to set.
     * @param b The blue value to set.
     * @return true if the color is successfully set to the device.
     */
    virtual bool setRgbLedColor(const uint8_t r, const uint8_t g, const uint8_t b);
    /**
     * @brief Returns the RGB LED color.
     * @param color The obtained color value.
     * @return true if the color is successfully obtained from the device.
     */
    bool getRgbLedColor(Color *color);
    /**
     * @brief Sets the RGB LED state.
     * @param state The state to set.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setRgbLedState(const RgbLedState state);
    /**
     * @brief Returns the RGB LED state.
     * @param state The obtained value for the RGB LED state.
     * @return true if the state is successfully obtained from the device.
     */
    bool getRgbLedState(RgbLedState *state);
    /**
     * @brief Sets the RGB LED state and color.
     * @param state The state to set.
     * @param color The color to set.
     * @return true if the state is successfully set to the device.
     */
    virtual bool setRgbLedState(const RgbLedState state, const Color color);

    /**
     * @brief Determines whether the pen tip is pressed or not.
     * @param pressed The tip state.
     * @return true if the state is successfully obtained from the device.
     */
    bool isTipPressed(bool* pressed);

    /**
     * @brief Returns the color value which the color sensor is last mesured.
     * @param color The obtained color value.
     */
    bool getColorValue(Color* color);

    /**
     * @brief Called when the value of the characteristic is changed.
     * @param characteristic The characteristic.
     * @param value The changed value.
     */
    void onCharacteristicValueChanged(const BleGattCharacteristic& characteristic,
                                      const BleGattCharacteristicValue& value) override;
    /**
     * @brief Called to process pen tip state messages.
     * @param value The characteristic value.
     */
    void onProcessTipStateChangeMessage(const BleGattCharacteristicValue& value);
    /**
     * @brief Dispatches the tip state changes.
     * @param tipPressed Whether the pen tip is pressed or not.
     */
    void dispatchTipStateChanged(const bool tipPressed);

    /**
     * @brief Called to process color value messages.
     * @param value The characteristic value.
     */
    void onProcessColorValueMessage(const BleGattCharacteristicValue& value);
    /**
     * @brief Dispatches the received color which is mesured by a color sensor.
     * @param color The color.
     */
    void dispatchColorValueReceived(const Color color);

    /**
     * @brief Attempts to subscribe server messages if a pen device is connected.
     */
    void reconnect();
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_BleIrPen_H_
