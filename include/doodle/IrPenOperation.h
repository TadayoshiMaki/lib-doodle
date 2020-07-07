#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenOperation_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenOperation_H_

#include "doodle/namespace.h"
#include "doodle/IrPenState.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPenOperation {
public:
    enum OperationType
    {
        UNKNOWN = 0,
        RGB_LED_STATE,
        RGB_LED_COLOR,
        IR_LED_STATE
    };

    IrPenOperation(OperationType type) : type_(type)
    {
    }
    virtual ~IrPenOperation()
    {
    }

    OperationType getType() const
    {
        return type_;
    }
private:
    OperationType type_;
};

class RgbLedStateOperation : public IrPenOperation
{
public:
    RgbLedStateOperation(const int deviceId, const RgbLedState state) :
        IrPenOperation(RGB_LED_STATE),
        deviceId_(deviceId),
        state_(state)
    {
    }
    virtual ~RgbLedStateOperation()
    {
    }

    RgbLedState getState() const
    {
        return state_;
    }

    /**
     * @brief Returns the device ID which this event is derived from.
    */
    int getDeviceId()
    {
        return deviceId_;
    }

private:
    // The device ID.
    const int deviceId_;
    // The rgb led state.
    RgbLedState state_;
};

class RgbLedColorOperation : public IrPenOperation
{
public:
    RgbLedColorOperation(const int deviceId, const Color color) :
        IrPenOperation(RGB_LED_COLOR),
        deviceId_(deviceId),
        color_(color)
    {
    }
    virtual ~RgbLedColorOperation()
    {
    }

    Color getColor() const
    {
        return color_;
    }
    /**
     * @brief Returns the device ID which this event is derived from.
    */
    int getDeviceId()
    {
        return deviceId_;
    }

private:
    // The device ID.
    const int deviceId_;
    // The rgb color.
    Color color_;
};

struct IrLedStateOperation : public IrPenOperation
{
    IrLedStateOperation(const int deviceId, const bool state) :
        IrPenOperation(IR_LED_STATE),
        deviceId_(deviceId),
        state_(state)
    {
    }
    virtual ~IrLedStateOperation()
    {
    }

    bool getState() const
    {
        return state_;
    }

    /**
     * @brief Returns the device ID which this event is derived from.
    */
    int getDeviceId()
    {
        return deviceId_;
    }

private:
    // The device ID.
    const int deviceId_;
    // The Ir led state.
    bool state_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenOperation_H_
