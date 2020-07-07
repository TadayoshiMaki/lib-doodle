#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenEventDispatcher_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenEventDispatcher_H_

#include <iostream>
#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/ScreenEvent.h"
#include "doodle/ScreenEventListener.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenEventDispatcher
{
public:
    ScreenEventDispatcher()
    {
    }

    virtual ~ScreenEventDispatcher()
    {
    }

    void addListener(const std::shared_ptr<ScreenEventListener>& l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const std::shared_ptr<ScreenEventListener>& l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it != listeners_.end())
        {
            listeners_.erase(it);
        }
    };

    /**
     * @brief Notifies of when screens are initialized.
     * @param e The screens initialization event.
     */
    void notifyScreenInit(const std::shared_ptr<ScreenInitEvent>& e)
    {
        {
            // TODO: This should be done on a worker thread.
            for (auto&& listener : listeners_)
            {
                listener->onScreenInitEvent(e);
            }
        }
    }

    /**
     * @bried Notifies of when the current screen is changed.
     * @param e The screen change event.
     */
    void notifyChangeCurrentScreen(const std::shared_ptr<ScreenChangeEvent>& e)
    {
        // TODO: This should be done on a worker thread.
        for (auto&& listener : listeners_)
        {
            listener->onScreenChangeEvent(e);
        }
    }

private:
    std::vector<std::shared_ptr<ScreenEventListener>> listeners_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenEventDispatcher_H_
