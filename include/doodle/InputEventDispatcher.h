#ifndef LIB_DOODLE_FRAMEWORK_doodle_InputEventDispatcher_H_
#define LIB_DOODLE_FRAMEWORK_doodle_InputEventDispatcher_H_

#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/InputEventListener.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class InputEventDispatcher
{
public:
    InputEventDispatcher()
    {
    }

    virtual ~InputEventDispatcher()
    {
    }

    void addListener(const std::shared_ptr<InputEventListener>& l)
    {
        auto& it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const std::shared_ptr<InputEventListener>& l)
    {
        auto& it = std::find(listeners_.begin(), listeners_.end(), l);
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
        // TODO: This should be done on a worker thread.
        for (auto&& listener : listeners_)
        {
            listener->onScreenInitEvent(e);
        }
    }

    /**
     * @bried Notifies of when the current screen is changed to another screen.
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

    /**
    * @brief Notifies of when pen devices are updated.
    * @param e The update event.
    */
    void notifyDeviceUpdate(const std::shared_ptr<PenDeviceUpdateEvent>& e)
    {
        // TODO: This should be done on a worker thread.
        for (auto&& listener : listeners_)
        {
            listener->onDeviceUpdateEvent(e);
        }
    }

    /**
     * @bried Notifies of a pen motion.
     * @param e The motion event.
     */
    void notifyMotion(const std::shared_ptr<PenMotionEvent>& e)
    {
        // TODO: This should be done on a worker thread.
        for (auto&& listener : listeners_)
        {
            listener->onMotionEvent(e);
        }
    }

    /**
     * @brief Notifies of when the color mesured with the color sensor of a pen.
     * @param e The color event.
     */
    void notifyColor(const std::shared_ptr<PenColorEvent>& e)
    {
        // TODO: This should be done on a worker thread.
        for (auto&& listener : listeners_)
        {
            listener->onColorEvent(e);
        }
    }

private:
    std::vector<std::shared_ptr<InputEventListener>> listeners_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_InputEventDispatcher_H_
