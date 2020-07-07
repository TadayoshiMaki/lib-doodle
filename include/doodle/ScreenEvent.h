#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenEvent_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenEvent_H_

#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/DoodleEvent.h"
#include "doodle/ScreenProperties.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenInitEvent : public DoodleEvent
{
public:
    ScreenInitEvent(std::vector<std::shared_ptr<ScreenProperty>> screens) :
        DoodleEvent(Type::SCREEN_INIT),
        screenProperties_(screens)
    {

    }

    virtual ~ScreenInitEvent()
    {
    }

    /**
     * @brief Returns the count of screens.
     */
    size_t getScreenCount() const
    {
        return screenProperties_.size();
    }

    /**
     * @brief Returns the specified screen.
     */
    std::shared_ptr<ScreenProperty> getScreen(const int index)
    {
        return screenProperties_.at(index);
    }

private:
    // The screen properties list.
    const std::vector<std::shared_ptr<ScreenProperty>> screenProperties_;
};



class ScreenChangeEvent : public DoodleEvent
{
public:
    enum Action
    {
        START,
        COMPLETE
    };

    ScreenChangeEvent(const Action action,
                      const int oldScreenId,
                      const int newScreenId) :
        DoodleEvent(Type::SCREEN_CHANGE),
        action_(action), 
        oldScreenId_(oldScreenId),
        newScreenId_(newScreenId)
    {
    }

    virtual ~ScreenChangeEvent()
    {
    }

    /**
     * @brief Returns the action of a screen change.
     */
    Action getAction()
    {
        return action_;
    }

    /**
     * @brief Returns the ID of a screen which a screen change starts from.
     */
    int getOldScreenId()
    {
        return oldScreenId_;
    }

    /**
     * @brief Returns the ID of a screen which a screen change gets to.
     */
    int getNewScreenId()
    {
        return newScreenId_;
    }
private:
    // The screen change action.
    const Action action_;
    // The ID of a screen which a screen change starts from.
    const int oldScreenId_;
    // The ID of a screen which a screen change gets to.
    const int newScreenId_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenEvent_H_
