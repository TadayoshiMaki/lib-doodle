#ifndef LIB_DOODLE_FRAMEWORK_doodle_CharacteristicValueChangeOperation_H_
#define LIB_DOODLE_FRAMEWORK_doodle_CharacteristicValueChangeOperation_H_

#include <memory>
#include <vector>

#include "ble_gatt_characteristic.h"
#include "doodle/namespace.h"
#include "doodle/IrPenOperation.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class CharacteristicValueChangeOperation
{
public:
    CharacteristicValueChangeOperation(
        const std::shared_ptr <IrPenOperation> operation,
        const std::shared_ptr<ble::BleGattCharacteristic> characteristic,
        const uint8_t* data,
        const size_t length) :
        operation_(operation),
        characteristic_(characteristic)
    {
        for (int i = 0; i < length; i++)
        {
            // Copy data so that it is used later in the operation.
            value_.push_back(data[i]);
        }
    }

    virtual ~CharacteristicValueChangeOperation()
    {
    }

    /**
     * @brief Executes the operation to change the characteristic value.
     * @return true if the operation is executed successfly.
     */
    bool run()
    {
        if (!characteristic_ || value_.empty()) return false;
        return characteristic_->setValue(value_.data(), value_.size());
    }

public:
    const std::shared_ptr<ble::BleGattCharacteristic> characteristic_;
    const std::shared_ptr <IrPenOperation> operation_;
private:
    std::vector<uint8_t> value_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_CharacteristicValueChangeOperation_H_
