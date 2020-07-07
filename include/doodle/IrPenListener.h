#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenListener_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenListener_H_

#include <memory>

#include "doodle/namespace.h"
#include "doodle/IrPenState.h"
#include "doodle/MemoryUtils.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPenListener
{
public:
    /**
     * @brief Called when the pen is resumed.
     * @param address The Bluetooth device address.
     */
    virtual void onResume(const std::string& address) = 0;
    /**
     * @brief Called when the pen is paused.
     * @param address The Bluetooth device address.
     */
    virtual void onPause(const std::string& address) = 0;
    /**
     * @brief Called when a client attempts to reconnect to the pen device.
     * @param address The Bluetooth device address.
     */
    virtual void onReconnect(const std::string& address) = 0;

    /**
     * @brief Called when the pen tip state is changed.
     * @param address The Bluetooth device address.
     * @param tipPressed Whether the pen tip is pressed or not.
     */
    virtual void onTipStateChanged(const std::string& address, const bool tipPressed) = 0;
    /**
     * @brief Called when the color value is received from the pen.
     * @param address The Bluetooth device address.
     * @param color The received color.
     */
    virtual void onColorValueReceived(const std::string& address, const Color color) = 0;
};



class IrPenStateHandler : public IrPenListener, public std::enable_shared_from_this<IrPenStateHandler>
{
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenListener_H_
