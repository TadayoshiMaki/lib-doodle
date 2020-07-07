#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenState_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenState_H_

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

enum RgbLedState
{
    kUnknown,
    kTurnedOn,
    kTurnedOff,
    kBlink
};

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color()
    {
    }

    Color(const uint8_t r, const uint8_t g, const uint8_t b) :
        r(r), g(g), b(b)
    {
    }
};

struct IrPenDeviceStatus
{
    int deviceId;
    RgbLedState rgbLedState;
    Color rgbLedColor;
    bool irLedState;

    IrPenDeviceStatus()
    {
    }

    IrPenDeviceStatus(const int deviceId,
                      const RgbLedState rgbLedState,
                      const Color rgbLedColor,
                      const bool irLedState) :
        deviceId(deviceId),
        rgbLedState(rgbLedState),
        rgbLedColor(rgbLedColor),
        irLedState(irLedState)
    {
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenState_H_
