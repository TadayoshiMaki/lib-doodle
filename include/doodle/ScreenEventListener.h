#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenEventListener_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenEventListener_H_

#include <memory>

#include "doodle/namespace.h"
#include "doodle/ScreenEvent.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenEventListener
{
public:
    /**
     * @brief Called when screens are initialized.
     * @param e The screens initialization event.
     */
    virtual void onScreenInitEvent(const std::shared_ptr<ScreenInitEvent>& e) = 0;

    /**
     * @brief Called when the current screen is changed.
     * @param e The screen change event.
     */
    virtual void onScreenChangeEvent(const std::shared_ptr<ScreenChangeEvent>& e) = 0;
};



class ScreenEventHandler : public ScreenEventListener, public std::enable_shared_from_this<ScreenEventHandler>
{
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenEventListener_H_
