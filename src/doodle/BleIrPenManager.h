#ifndef LIB_DOODLE_FRAMEWORK_doodle_BleIrPenManager_H_
#define LIB_DOODLE_FRAMEWORK_doodle_BleIrPenManager_H_

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "ble_device.h"
#include "ble_gatt_characteristic.h"
#include "doodle/namespace.h"
#include "doodle/BleIrPen.h"
#include "doodle/IrPenListener.h"
#include "doodle/IrPenOperation.h"
#include "doodle/IrPenManager.h"
#include "doodle/CharacteristicValueChangeOperation.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class BleIrPenManager final : public IrPenManager
{
public:
    BleIrPenManager();
    virtual ~BleIrPenManager();

    virtual void setDevicesUpdatedCallback(const DevicesUpdatedCallback& callback) override
    {
        devicesUpdatedCallback_ = std::make_shared<DevicesUpdatedCallback>(callback);
    }

    virtual void setStateChangedCallback(const StateChangedCallback& callback) override
    {
        stateChangedCallback_ = std::make_shared<StateChangedCallback>(callback);
    }

    virtual void setColorReceivedCallback(const ColorReceivedCallback& callback) override
    {
        colorReceivedCallback_ = std::make_shared<ColorReceivedCallback>(callback);
    }

    virtual void setRgbLedStateChangedCallback(const RgbLedStateChangedCallback& callback) override
    {
        RgbLedStateChangedCallback_ = std::make_shared<RgbLedStateChangedCallback>(callback);
    }

    virtual void setRgbLedColorChangedCallback(const RgbLedColorChangedCallback& callback) override
    {
        RgbLedColorChangedCallback_ = std::make_shared<RgbLedColorChangedCallback>(callback);
    }

    virtual void setIrLedStateChangedCallback(const IrLedStateChangedCallback& callback) override
    {
        IrLedStateChangedCallback_ = std::make_shared<IrLedStateChangedCallback>(callback);
    }

    virtual void initialize() override;
    virtual void finalize() override;
    virtual size_t getCount() override;
    virtual std::vector<int> getDeviceIds() override;
    virtual std::vector<int> getActiveDeviceIds() override;
    //virtual int getDeviceId(const std::shared_ptr<IrPen>& pen) override;
    virtual std::shared_ptr<IrPen> getPen(const int deviceId) override;
    virtual bool isConnected(const int deviceId) override;
    //virtual bool isConnected(const std::shared_ptr<IrPen>& pen) override;
    virtual bool empty() override;
    virtual std::vector<IrPenDeviceStatus> getDeviceStatusList() override;
    virtual IrPenDeviceStatus getDeviceStatus(const int deviceId) override;
    virtual bool resume(const int deviceId) override;
    virtual void pause(const int deviceId) override;
    virtual std::string getAddress(const int deviceId) override;

    /**
     * @brief Returns the BLE device underlying the IR pen.
     */
    std::shared_ptr<ble::BleDevice> getDevice(const int deviceId);

    /**
     * @brief Returns the characteristic corresponding to the specified UUID.
     */
    std::shared_ptr<ble::BleGattCharacteristic> getCharacteristic(const int deviceId, const std::string& uuid);

    virtual bool setIrLedState(const int deviceId, const bool enable) override;
    virtual bool getIrLedState(const int deviceId, bool* enable) override;
    virtual bool setRgbLedColor(const int deviceId, const Color color) override;
    virtual bool setRgbLedColor(const int deviceId, const uint8_t r, const uint8_t g, const uint8_t b) override;
    virtual bool getRgbLedColor(const int deviceId, Color *color) override;
    virtual bool setRgbLedState(const int deviceId, const RgbLedState state) override;
    virtual bool getRgbLedState(const int deviceId, RgbLedState *state) override;
    virtual bool setRgbLedState(const int deviceId, const RgbLedState state, const Color color) override;
    virtual bool isTipPressed(const int deviceId, bool* pressed) override;
    virtual bool getColorValue(const int deviceId, Color* color) override;
    virtual void resetState(const int deviceId) override;

private:
    // Used to assign IDs to devices.
    std::shared_ptr<DeviceIdGenerator> idGenerator_;

    std::shared_ptr<DevicesUpdatedCallback> devicesUpdatedCallback_;
    std::shared_ptr<StateChangedCallback> stateChangedCallback_;
    std::shared_ptr<ColorReceivedCallback> colorReceivedCallback_;
    std::shared_ptr<RgbLedStateChangedCallback> RgbLedStateChangedCallback_;
    std::shared_ptr<RgbLedColorChangedCallback> RgbLedColorChangedCallback_;
    std::shared_ptr<IrLedStateChangedCallback> IrLedStateChangedCallback_;

    // The map for pairs of an ID and a device.
    std::map<const int, std::shared_ptr<BleIrPen>, std::less<>> entries_;

    /**
     * @brief Called when the pen is resumed.
     * @param address The Bluetooth device address.
     */
    void onResume(const std::string& address);
    /**
     * @brief Called when the pen is paused.
     * @param address The Bluetooth device address.
     */
    void onPause(const std::string& address);
    /**
     * @brief Called when a client attempts to reconnect to the pen device.
     * @param address The Bluetooth device address.
     */
    void onReconnect(const std::string& address);

    // The worker thread to execute operations.
    std::thread thread_;
    std::condition_variable queueCondition_;
    std::mutex queueMutex_;
    std::queue<std::shared_ptr<CharacteristicValueChangeOperation>> queue_;

    // Indicates whether the worker thread is started or not.
    std::atomic_bool started_ = false;
    // Indicates whether the worker thread should exit the loop.
    std::atomic_bool exitLoop_ = false;

    /**
     * @bried Called when the state of a pen tip is changed.
     * @param address The address of a pen device.
     * @param tipPressed Whether the pen tip is pressed or not.
     */
    void onTipStateChanged(const std::string& address, const bool tipPressed);

    /**
     * @brief Called when the color mesured with the color sensor of a pen.
     * @param address The address of a pen device.
     * @param color The RGB color in 16bit.
     */
    void onColorValueReceived(const std::string& address, const Color color);

    /**
     * @brief Starts the worker thread.
     */
    void start();

    /**
     * @brief Stops the worker thread.
     */
    void stop();

    /**
     * @brief Determines whether the worker thread is started or not.
     */
    bool isStarted();

    /**
     * @brief Processes operations in a loop.
     */
    void loop();

    int getDeviceId(const std::shared_ptr<BleIrPen>& pen);
    std::shared_ptr<BleIrPen> getBlePen(const int deviceId);
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_BleIrPenManager_H_
