#ifndef LIB_DOODLE_FRAMEWORK_doodle_DoodleEvent_H_
#define LIB_DOODLE_FRAMEWORK_doodle_DoodleEvent_H_

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class DoodleEvent
{
public:
    enum Type
    {
        PEN_DEVICE_UPDATE,
        PEN_ACTIVE,
        PEN_INACTIVE,
        PEN_TIMEOUT,
        PEN_MOTION,
        PEN_COLOR,
        SCREEN_INIT,
        SCREEN_CHANGE
    };

    /**
     * @brief Returns the type of an event.
     */
    Type getType() const
    {
        return type_;
    }

protected:
    DoodleEvent(Type type) : type_(type)
    {
    }

    virtual ~DoodleEvent()
    {
    }

private:
    // The type of this event.
    const Type type_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_DoodleEvent_H_
