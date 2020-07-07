#ifndef LIB_DOODLE_FRAMEWORK_doodle_StrokeStatus_H_
#define LIB_DOODLE_FRAMEWORK_doodle_StrokeStatus_H_

#include "doodle/namespace.h"
#include "doodle/IrPenEvent.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class StrokeStatus
{
public:
    StrokeStatus(int deviceId, bool active, PenMotionEvent::Action nextAction, bool expired = true) :
        deviceId_(deviceId),
        active_(active),
        nextAction_(nextAction),
        expired_(expired)
    {
    }

    virtual ~StrokeStatus()
    {
    }

    int getDeviceId()
    {
        return deviceId_;
    }

    void setActive(bool active)
    {
        active_ = active;
    }

    bool isActive()
    {
        return active_;
    }

    void setNextAction(PenMotionEvent::Action nextAction)
    {
        nextAction_ = nextAction;
    }

    PenMotionEvent::Action getNextAction()
    {
        return nextAction_;
    }

    void setExpired(bool expired)
    {
        expired_ = expired;
    }

    bool isExpired()
    {
        return expired_;
    }

    void setLastMotionEvent(const std::shared_ptr<PenMotionEvent> e)
    {
        lastEvent_ = e;
    }

    unsigned char getLastTrackingId()
    {
        // TODO: No last event.
        if (!lastEvent_) return 0;
        return lastEvent_->getTrackingId();
    }

    float getLastX()
    {
        // TODO: No last event.
        if (!lastEvent_) return 0;
        return lastEvent_->getX();
    }

    float getLastY()
    {
        // TODO: No last event.
        if (!lastEvent_) return 0;
        return lastEvent_->getY();
    }

    bool isFirstDownAction()
    {
        return (lastEvent_->getAction() == PenMotionEvent::Action::DOWN && expired_);
    }

    float getEuclideanDistance(const std::shared_ptr<PenMotionEvent>& e)
    {
        if (!lastEvent_) return 0.f;
        return std::hypot(lastEvent_->getX() - e->getX(), lastEvent_->getY() - e->getY());
    }

private:
    // The source device ID.
    const int deviceId_;
    // Whether the device is active or not.
    bool active_;
    // The action corresponding to a next motion event.
    PenMotionEvent::Action nextAction_;
    // Whether this stroke is expired or not.
    bool expired_;

    // The previous motion event.
    std::shared_ptr<PenMotionEvent> lastEvent_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_StrokeStatus_H_
