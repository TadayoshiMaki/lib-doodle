#include "doodle/BleIrPen.h"

#include "ble_device_manager.h"
#include "ble_utils.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

BleIrPen::BleIrPen(const std::shared_ptr<ble::BleDevice>& device) :
    resumed_(false),
    device_(device),
    autoReconnectionEnabled_(true)
{
    service_ = device->getService(kServiceUuid);
    initCharacteristics();
}

BleIrPen::~BleIrPen()
{
    characteristicMap_.clear();
}

bool BleIrPen::resume()
{
    std::lock_guard<std::mutex> lock(mutex_);
    resumed_ = true;
    bool success = subscribeServerMessages();
    onResume();
    dispatchResume();
    return success;
}

void BleIrPen::pause()
{
    std::lock_guard<std::mutex> lock(mutex_);
    resumed_ = false;
    unsubscribeServerMessages();
    onPause();
    dispatchPause();
}

bool BleIrPen::isResumed()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return resumed_;
}

std::string BleIrPen::getAddress()
{
    return device_->getAddress();
}

bool BleIrPen::setIrLedState(const bool enable)
{
    auto& characteristic = getCharacteristic(kIrLedConfigCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isWritable()) return false;

    uint8_t value = enable ? 0x01 : 0x00;
    uint8_t data[1] = { value };
    return characteristic->setValue(data, 1);
}

bool BleIrPen::getIrLedState(bool* enable)
{
    if (!enable) return false;

    auto& characteristic = getCharacteristic(kIrLedConfigCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isReadable()) return false;

    BleGattCharacteristicValue value;
    bool success = characteristic->getValue(&value);
    if (value.size() == 1)
    {
        const uint8_t* data = value.getData();
        switch (data[0])
        {
        case 0x00:
            *enable = false;
            break;
        case 0x01:
        default:
            *enable = true;
            break;
        }
    }
    else
    {
        // Not initialized.
        success = false;
    }

    return success;
}

bool BleIrPen::setRgbLedColor(const Color color)
{
    return setRgbLedColor(color.r, color.g, color.b);
}

bool BleIrPen::setRgbLedColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    auto& characteristic = getCharacteristic(kRgbLedColorCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isWritable()) return false;

    uint8_t data[3] = { r, g, b };
    // TODO: Update the internal state if succeeded.
    return characteristic->setValue(data, 3);
}

bool BleIrPen::getRgbLedColor(Color* color)
{
    if (!color) return false;

    auto& characteristic = getCharacteristic(kRgbLedColorCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isReadable()) return false;

    BleGattCharacteristicValue value;
    bool success = characteristic->getValue(&value);
    if (value.size() == 3)
    {
        const uint8_t* data = value.getData();
        color->r = data[0];
        color->g = data[1];
        color->b = data[2];
    }
    else
    {
        // Not initialized.
        success = false;
    }

    return success;
}

bool BleIrPen::setRgbLedState(const RgbLedState state)
{
    auto& characteristic = getCharacteristic(kRgbLedConfigCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isWritable()) return false;

    uint8_t data[1];
    switch (state)
    {
    case RgbLedState::kTurnedOn:
        data[0] = 0x01;
        break;
    case RgbLedState::kTurnedOff:
        data[0] = 0x00;
        break;
    case RgbLedState::kBlink:
        data[0] = 0x02;
        break;
    case RgbLedState::kUnknown:
    default:
        data[0] = 0x00;
        break;
    }
    // TODO: Update the internal state if succeeded.
    return characteristic->setValue(data, 1);
}

bool BleIrPen::getRgbLedState(RgbLedState* state)
{
    auto& characteristic = getCharacteristic(kRgbLedConfigCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isReadable()) return false;

    BleGattCharacteristicValue value;
    bool success = characteristic->getValue(&value);
    if (value.size() == 1)
    {
        const uint8_t* data = value.getData();
        switch (data[0])
        {
        case 0x00:
            *state = kTurnedOff;
            break;
        case 0x01:
            *state = kTurnedOn;
            break;
        case 0x02:
            *state = kBlink;
            break;
        default:
            // TODO: Implement the blink function.
            *state = kUnknown;
            break;
        }
    }
    else
    {
        // Not initialized.
        success = false;
    }

    return success;
}

bool BleIrPen::setRgbLedState(const RgbLedState state, const Color color)
{
    bool success = setRgbLedColor(color);
    success &= setRgbLedState(state);
    return success;
}

bool BleIrPen::isTipPressed(bool* pressed)
{
    auto& characteristic = getCharacteristic(kTipStateCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isReadable()) return false;

    BleGattCharacteristicValue value;
    bool success = characteristic->getValue(&value);
    if (value.size() == 1)
    {
        const uint8_t* data = value.getData();
        switch (data[0])
        {
        case 0x00:
            *pressed = false;
            break;
        case 0x01:
        default:
            *pressed = true;
            break;
        }
    }
    else
    {
        // Not initialized.
        success = false;
    }

    return success;
}

bool BleIrPen::getColorValue(Color* color)
{
    auto& characteristic = getCharacteristic(kColorPickerCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isReadable()) return false;

    BleGattCharacteristicValue value;
    bool success = characteristic->getValue(&value);
    if (value.size() == 4)
    {
        const uint8_t* data = value.getData();
        color->r = data[0];
        color->g = data[1];
        color->b = data[2];
    }
    else
    {
        // Not initialized.
        success = false;
    }

    return success;
}

void BleIrPen::resetState()
{
    // Reset the state of the pen if connected.
    if (isConnected())
    {
        // Deactivate pens to unsubscribe notifications if connected.
        setIrLedState(false);
        setRgbLedState(RgbLedState::kTurnedOff);
        pause();
    }
}

void BleIrPen::onResume()
{
    std::lock_guard<std::mutex> lock(reconnectionMutex_);
    if (reconnector_) reconnector_->stop();
    reconnector_ = std::make_unique<Ticker>(kAutoReconnectIntervalMillis);
    reconnector_->start(this, &BleIrPen::reconnect);
}

void BleIrPen::onPause()
{
    std::lock_guard<std::mutex> lock(reconnectionMutex_);
    if (reconnector_) reconnector_->stop();
}

bool BleIrPen::subscribeServerMessages()
{
    bool success = false;
    for (auto&& entry : characteristicMap_)
    {
        auto& characteristic = entry.second;
        if (characteristic->isNotifiable())
        {
            characteristic->addListener(shared_from_this());
            success |= characteristic->setNotificationEnabled(true);
            if (isConnected() && !success)
            {
                // Retry to enable notifications if connected to a peripheral.
                characteristic->ensureNotificationStatus();
            }
        }
        else if (characteristic->isIndicatable())
        {
            characteristic->addListener(shared_from_this());
            success |= characteristic->setIndicationEnabled(true);
            if (isConnected() && !success)
            {
                // Retry to enable indications if connected to a peripheral.
                characteristic->ensureIndicationStatus();
            }
        }
    }
    return success;
}

void BleIrPen::unsubscribeServerMessages()
{
    for (auto&& entry : characteristicMap_)
    {
        auto& characteristic = entry.second;
        if (characteristic->isNotifiable())
        {
            characteristic->setNotificationEnabled(false);
            characteristic->removeListener(shared_from_this());
        }
        else if (characteristic->isIndicatable())
        {
            characteristic->setIndicationEnabled(false);
            characteristic->removeListener(shared_from_this());
        }
    }
}

void BleIrPen::onReconnect()
{
    bool success = false;
    if (isResumed() && !isConnected())
    {
        success = subscribeServerMessages();
    }

    if (success)
    {
        // This should be called after reconnecting successfully.
        dispatchReconnect();
    }

}

void BleIrPen::initCharacteristics()
{
    auto characteristics = service_->getCharacteristics();
    for (auto&& characteristic : characteristics)
    {
        std::string guidString = guidToString(characteristic->getUuid());
        BleGattCharacteristicEntry entry = std::make_pair(guidString, characteristic);
        characteristicMap_.insert(entry);
    }
}

void BleIrPen::dispatchResume()
{
    foreachListeners([&](std::shared_ptr<IrPenListener> l) {
        l->onResume(device_->getAddress());
    });
}

void BleIrPen::dispatchPause()
{
    foreachListeners([&](std::shared_ptr<IrPenListener> l) {
        l->onPause(device_->getAddress());
    });
}

void BleIrPen::dispatchReconnect()
{
    foreachListeners([&](std::shared_ptr<IrPenListener> l) {
        l->onReconnect(device_->getAddress());
    });
}

void BleIrPen::dispatchTipStateChanged(const bool tipPressed)
{
    foreachListeners([&](std::shared_ptr<IrPenListener> l) {
        l->onTipStateChanged(device_->getAddress(), tipPressed);
    });
}

void BleIrPen::dispatchColorValueReceived(const Color color)
{
    foreachListeners([&](std::shared_ptr<IrPenListener> l) {
        l->onColorValueReceived(device_->getAddress(), color);
    });
}

void BleIrPen::onCharacteristicValueChanged(
    const BleGattCharacteristic& characteristic, const BleGattCharacteristicValue& value)
{
    if (characteristic.uuidEquals(IrPen::kTipStateCharacteristicUuid))
    {
        onProcessTipStateChangeMessage(value);
    }
    else if (characteristic.uuidEquals(IrPen::kColorPickerCharacteristicUuid))
    {
        onProcessColorValueMessage(value);
    }
}

void BleIrPen::onProcessTipStateChangeMessage(const BleGattCharacteristicValue& value)
{
    const UCHAR* data = value.getData();
    const ULONG size = value.size();
    if (size != 1) return;

    bool pressed = false;
    switch (data[0])
    {
    case 0x00:
        pressed = false;
        break;
    case 0x01:
    default:
        pressed = true;
        break;
    }

    dispatchTipStateChanged(pressed);
}

void BleIrPen::onProcessColorValueMessage(const BleGattCharacteristicValue& value)
{
    const UCHAR* data = value.getData();
    const ULONG size = value.size();
    if (size != 4) return;

    Color color;
    color.r = data[0];
    color.g = data[1];
    color.b = data[2];

    dispatchColorValueReceived(color);
}

void BleIrPen::reconnect()
{
    std::lock_guard<std::mutex> lock(reconnectionMutex_);
    if (autoReconnectionEnabled_) onReconnect();
}

std::shared_ptr<BleDevice> BleIrPen::getDevice()
{
    return device_;
}

bool BleIrPen::isConnected()
{
    return BleDeviceManager::isConnected(device_);
}

std::shared_ptr<BleGattCharacteristic> BleIrPen::getCharacteristic(const std::string& uuid)
{
    if (characteristicMap_.count(uuid) <= 0) return nullptr;
    return characteristicMap_.at(uuid);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
