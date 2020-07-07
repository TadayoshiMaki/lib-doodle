#ifndef LIB_DOODLE_FRAMEWORK_doodle_InputEventListener_H_
#define LIB_DOODLE_FRAMEWORK_doodle_InputEventListener_H_

#include <memory>

#include "doodle/namespace.h"
#include "doodle/IrPenEvent.h"
#include "doodle/ScreenEventListener.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class InputEventListener : public ScreenEventListener
{
public:
    /**
    * @brief Called when pen devices are updated.
    * @param e The update event.
    */
    virtual void onDeviceUpdateEvent(const std::shared_ptr<PenDeviceUpdateEvent>& e) = 0;
    /**
    * @bried Called when a pen motion is received.
    * @param e The motion event.
    */
    virtual void onMotionEvent(const std::shared_ptr<PenMotionEvent>& e) = 0;
    /**
    * @brief Called when the color is mesured with the color sensor of a pen.
    * @param e The color event.
    */
    virtual void onColorEvent(const std::shared_ptr<PenColorEvent>& e) = 0;
};


class InputEventHandler : public InputEventListener, public std::enable_shared_from_this<InputEventHandler>
{
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_InputEventListener_H_
