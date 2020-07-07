#include <iostream>

#include "doodle/BleIrPenManager.h"
#include "ble_device_manager.h"
//#include "doodle/CharacteristicValueChangeOperation.h"

//#include "async_ir_pen.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

BleIrPenManager::BleIrPenManager()
{
    idGenerator_ = std::make_shared<DeviceIdGenerator>();
    start();
}

BleIrPenManager::~BleIrPenManager()
{
    stop();

    if (thread_.joinable())
    {
        thread_.join();
    }
}

void BleIrPenManager::initialize()
{
    // Remove previously detected pens.
    finalize();
    auto devices = ble::BleDeviceManager::getKnownDevices(IrPen::kServiceUuid);
    for (auto&& device : devices)
    {
        auto pen = std::make_shared<BleIrPen>(device);
        const auto& address = pen->getAddress();
        // IDs should be generated with pen registration.
        const int deviceId = idGenerator_->generate(address);
        // Register connected (paired with the computer) pens to the manager.
        entries_.insert(std::make_pair(deviceId, pen));
        pen->addListener(shared_from_this());
        // Activate pens to subscribe notifications.
        pen->resume();
    }

    auto statusList = getDeviceStatusList();
    for (auto&& status : statusList)
    {
        // Overwrite the current RGB LED color.
        status.rgbLedColor = kInitialRgbLedColor;
    }

    if (devicesUpdatedCallback_) (*devicesUpdatedCallback_)(std::move(statusList));
}

void BleIrPenManager::finalize()
{
    for (auto& it = entries_.begin(); it != entries_.end(); ++it)
    {
        auto pen = it->second;
        pen->resetState();
        pen->removeListener(shared_from_this());
    }
    entries_.clear();
}

size_t BleIrPenManager::getCount()
{
    return entries_.size();
}

std::vector<int> BleIrPenManager::getDeviceIds()
{
    return std::move(idGenerator_->getIds());
}

std::vector<int> BleIrPenManager::getActiveDeviceIds()
{
    std::vector<int> activePenIds;
    for (auto& it = entries_.begin(); it != entries_.end(); ++it)
    {
        activePenIds.push_back(getDeviceId(it->second));
    }
    return std::move(activePenIds);
}

//int BleIrPenManager::getDeviceId(const std::shared_ptr<IrPen>& pen)
//{
//    return idGenerator_->get(pen->getAddress());
//}

int BleIrPenManager::getDeviceId(const std::shared_ptr<BleIrPen>& pen)
{
    return idGenerator_->get(pen->getAddress());
}

std::shared_ptr<BleIrPen> BleIrPenManager::getBlePen(const int deviceId)
{
    auto& it = entries_.find(deviceId);
    if (it != entries_.end())
    {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<IrPen> BleIrPenManager::getPen(const int deviceId)
{
    return getBlePen(deviceId);
}

bool BleIrPenManager::isConnected(const int deviceId)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->isConnected();
}

//bool BleIrPenManager::isConnected(const std::shared_ptr<IrPen>& pen)
//{
//    return pen->isConnected();
//}

bool BleIrPenManager::empty()
{
    return entries_.empty();
}

std::vector<IrPenDeviceStatus> BleIrPenManager::getDeviceStatusList()
{
    std::vector<IrPenDeviceStatus> status;
    for (auto&& entry : entries_)
    {
        auto deviceId = entry.first;
        auto pen = entry.second;
        RgbLedState rgbLedState;
        Color rgbLedColor;
        bool irLedState;

        pen->getRgbLedState(&rgbLedState);
        pen->getRgbLedColor(&rgbLedColor);
        pen->getIrLedState(&irLedState);

        IrPenDeviceStatus deviceState{ deviceId, rgbLedState, rgbLedColor, irLedState };
        status.push_back(deviceState);
    }
    return std::move(status);
}

IrPenDeviceStatus BleIrPenManager::getDeviceStatus(const int deviceId)
{
    IrPenDeviceStatus status;
    for (auto&& entry : entries_)
    {
        if (deviceId == entry.first)
        {
            auto pen = entry.second;
            RgbLedState rgbLedState;
            Color rgbLedColor;
            bool irLedState;

            pen->getRgbLedState(&rgbLedState);
            pen->getRgbLedColor(&rgbLedColor);
            pen->getIrLedState(&irLedState);

            status.deviceId = deviceId;
            status.rgbLedState = rgbLedState;
            status.rgbLedColor = rgbLedColor;
            status.irLedState = irLedState;
            break;
        }
    }
    return std::move(status);
}

void BleIrPenManager::onResume(const std::string& address)
{
}

void BleIrPenManager::onPause(const std::string& address)
{
}

void BleIrPenManager::onReconnect(const std::string& address)
{
    int deviceId = idGenerator_->get(address);
    if (deviceId == kInvalidDeviceId) return;

    IrPenDeviceStatus status = getDeviceStatus(deviceId);
    std::vector<IrPenDeviceStatus> statusList = { status };

    if (devicesUpdatedCallback_) (*devicesUpdatedCallback_)(std::move(statusList));
}

bool BleIrPenManager::resume(const int deviceId)
{
    auto pen = getPen(deviceId);
    if (pen) {
        return pen->resume();
    }
    return false;
}

void BleIrPenManager::pause(const int deviceId)
{
    auto pen = getPen(deviceId);
    if (pen) {
        pen->pause();
    }
    return;
}

std::string BleIrPenManager::getAddress(const int deviceId)
{
    auto pen = getBlePen(deviceId);
    if (pen) {
        return pen->getAddress();
    }
    return std::move(std::string());
}

//std::shared_ptr<BleDevice> BleIrPenManager::getDevice(const int deviceId)
//{
//    auto pen = getPen(deviceId);
//    if (pen) {
//        return pen->getDevice();
//    }
//    return std::move(std::shared_ptr<BleDevice>());
//}

//std::shared_ptr<BleGattCharacteristic> BleIrPenManager::getCharacteristic(const int deviceId, const std::string & uuid)
//{
//    auto pen = getPen(deviceId);
//    if (pen) {
//        return pen->getCharacteristic(uuid);
//    }
//    return std::move(std::shared_ptr<BleGattCharacteristic>());
//}

bool BleIrPenManager::setIrLedState(const int deviceId, const bool enable)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    auto characteristic = pen->getCharacteristic(IrPen::kIrLedConfigCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isWritable()) return false;

    uint8_t value = enable ? 0x01 : 0x00;
    uint8_t data[1] = { value };
    std::shared_ptr<IrPenOperation> operation;
    operation = std::make_shared<IrLedStateOperation>(deviceId, enable);
    auto op = std::make_shared<CharacteristicValueChangeOperation>(operation, characteristic, data, 1);
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push(std::move(op));
        queueCondition_.notify_one();
    }
    return true;
}

bool BleIrPenManager::getIrLedState(const int deviceId, bool* enable)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->getIrLedState(enable);
}

bool BleIrPenManager::setRgbLedColor(const int deviceId, const Color color)
{
    return setRgbLedColor(deviceId, color.r, color.g, color.b);
}

bool BleIrPenManager::setRgbLedColor(const int deviceId, const uint8_t r, const uint8_t g, const uint8_t b)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    auto characteristic = pen->getCharacteristic(IrPen::kRgbLedColorCharacteristicUuid);
    if (!characteristic) return false;
    // Invalid characteristic property.
    if (!characteristic->isWritable()) return false;

    uint8_t data[3] = { r, g, b };
    std::shared_ptr<IrPenOperation> operation;
    operation = std::make_shared<RgbLedColorOperation>(deviceId, Color(r, g, b));
    auto op = std::make_shared<CharacteristicValueChangeOperation>(operation, characteristic, data, 3);
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push(std::move(op));
        queueCondition_.notify_one();
    }
    return true;
}

bool BleIrPenManager::getRgbLedColor(const int deviceId, Color* color)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->getRgbLedColor(color);
}

bool BleIrPenManager::setRgbLedState(const int deviceId, const RgbLedState state)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    auto characteristic = pen->getCharacteristic(IrPen::kRgbLedConfigCharacteristicUuid);
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
    std::shared_ptr<IrPenOperation> operation;
    operation = std::make_shared<RgbLedStateOperation>(deviceId, state);
    auto op = std::make_shared<CharacteristicValueChangeOperation>(operation, characteristic, data, 1);
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push(std::move(op));
        queueCondition_.notify_one();
    }
    return true;
}

bool BleIrPenManager::getRgbLedState(const int deviceId, RgbLedState* state)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->getRgbLedState(state);
}

bool BleIrPenManager::setRgbLedState(const int deviceId, const RgbLedState state, const Color color)
{
    bool success = setRgbLedColor(deviceId, color);
    success &= setRgbLedState(deviceId, state);
    return success;
}

bool BleIrPenManager::isTipPressed(const int deviceId, bool* pressed)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->isTipPressed(pressed);
}

bool BleIrPenManager::getColorValue(const int deviceId, Color* color)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return false;
    return pen->getColorValue(color);
}

void BleIrPenManager::resetState(const int deviceId)
{
    auto pen = getBlePen(deviceId);
    if (!pen) return;
    return pen->resetState();
}

void BleIrPenManager::onTipStateChanged(const std::string& address, const bool tipPressed)
{
    int deviceId = idGenerator_->get(address);
    if (deviceId == kInvalidDeviceId) return;

    if (stateChangedCallback_)
    {
        (*stateChangedCallback_)(deviceId, tipPressed);
    }
}

void BleIrPenManager::onColorValueReceived(const std::string& address, const Color color)
{
    int deviceId = idGenerator_->get(address);
    if (deviceId == kInvalidDeviceId) return;

    if (colorReceivedCallback_)
    {
        (*colorReceivedCallback_)(deviceId, color);
    }
}


void BleIrPenManager::start()
{
    if (isStarted()) return;

    thread_ = std::thread([this] { loop(); });
}

void BleIrPenManager::stop()
{
    if (!isStarted()) return;

    exitLoop_ = true;
    queueCondition_.notify_one();
}

bool BleIrPenManager::isStarted()
{
    return started_;
}

void BleIrPenManager::loop()
{
    started_.store(true);
    std::shared_ptr<CharacteristicValueChangeOperation> op;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            while (queue_.empty())
            {
                if (exitLoop_.load())
                {
                    break;
                }
                queueCondition_.wait(lock);
            }

            if (exitLoop_.load())
            {
                break;
            }

            if (queue_.empty())
            {
                continue;
            }

            op = queue_.front();
            queue_.pop();
        }
        // TODO: Should ignore operations if no connection.
        if (op) {
            if (!op->run()) {
                std::cout << "ir pen manager operation error" << std::endl;
                continue;
            }
            //If success, callback
            const auto& operation = op->operation_;
            switch (operation->getType())
            {
            case IrPenOperation::RGB_LED_STATE:
            {
                auto o = std::dynamic_pointer_cast<RgbLedStateOperation>(operation);
                if (RgbLedStateChangedCallback_) { (*RgbLedStateChangedCallback_)(o->getDeviceId(), o->getState()); }
            }
            break;
            case IrPenOperation::RGB_LED_COLOR:
            {
                auto o = std::dynamic_pointer_cast<RgbLedColorOperation>(operation);
                if (RgbLedColorChangedCallback_) { (*RgbLedColorChangedCallback_)(o->getDeviceId(), o->getColor()); }
            }
            break;
            case IrPenOperation::IR_LED_STATE:
            {
                auto o = std::dynamic_pointer_cast<IrLedStateOperation>(operation);
                if (IrLedStateChangedCallback_) { (*IrLedStateChangedCallback_)(o->getDeviceId(), o->getState()); }
            }
            break;
            }
        }
    }
    started_.store(false);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
