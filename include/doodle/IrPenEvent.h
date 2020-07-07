#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenEvent_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenEvent_H_

#include "doodle/namespace.h"
#include "doodle/DoodleEvent.h"
#include "doodle/HsvSensor.h"
#include "doodle/IrPenState.h"
#include "doodle/IrPenManager.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class PenActiveEvent : public DoodleEvent
{
public:
    PenActiveEvent(const int deviceId) :
        DoodleEvent(Type::PEN_ACTIVE),
        deviceId_(deviceId)
    {
    }

    virtual ~PenActiveEvent()
    {
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
};



class PenInactiveEvent : public DoodleEvent
{
public:
    PenInactiveEvent(const int deviceId) :
        DoodleEvent(Type::PEN_INACTIVE),
        deviceId_(deviceId)
    {
    }

    virtual ~PenInactiveEvent()
    {
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
};



class PenTimeoutEvent : public DoodleEvent
{
public:
    PenTimeoutEvent(const int deviceId) :
        DoodleEvent(Type::PEN_TIMEOUT),
        deviceId_(deviceId)
    {
    }

    virtual ~PenTimeoutEvent()
    {
    }

    /**
     * @brief Returns the device ID which this event is derived from.
     */
    const int getDeviceId()
    {
        return deviceId_;
    }

private:
    // The device ID.
    const int deviceId_;
};



class PenDeviceUpdateEvent : public DoodleEvent
{
public:
    PenDeviceUpdateEvent(const std::vector<IrPenDeviceStatus> statusList) :
        DoodleEvent(Type::PEN_DEVICE_UPDATE),
        statusList_(statusList)
    {
    }

    virtual ~PenDeviceUpdateEvent()
    {
    }

    /**
     * @brief Returns the count of pen devices.
     */
    size_t getDeviceCount()
    {
        return statusList_.size();
    }

    /**
     * @brief Returns the state of the specified device.
     */
    IrPenDeviceStatus getDeviceStatus(const int index) const
    {
        return statusList_.at(index);
    }

private:
    // The state list of devices.
    const std::vector<IrPenDeviceStatus> statusList_;
};



// TODO: Should be abstract like MotionEvent.
class PenMotionEvent : public DoodleEvent
{
public:
    enum Action
    {
        NONE,
        DOWN,
        UP,
        CANCEL
    };

    static const int kInvalidDeviceId = IrPenManager::kInvalidDeviceId;

    PenMotionEvent(const int deviceId,
        const HsvSensor::SensingResult& result) :
        PenMotionEvent(deviceId, Action::NONE, result.trackingId, result.sensingState, result.x, result.y, result.timeMicros)
    {
    }

    PenMotionEvent(const int deviceId,
        const Action action,
        const unsigned char trackingId,
        const float x,
        const float y,
        const int64_t timeMicros) :
        DoodleEvent(Type::PEN_MOTION),
        deviceId_(deviceId),
        action_(action),
        trackingId_(trackingId),
        x_(x),
        y_(y),
        timeMicros_(timeMicros)
    {
    }

    PenMotionEvent(const int deviceId,
        const Action action,
        const unsigned char trackingId,
        const HsvSensor::SensingState sensingState,
        const float x,
        const float y,
        const int64_t timeMicros) :
        DoodleEvent(Type::PEN_MOTION),
        deviceId_(deviceId),
        action_(action),
        trackingId_(trackingId),
        sensingState_(sensingState),
        x_(x),
        y_(y),
        timeMicros_(timeMicros)
    {
    }


    virtual ~PenMotionEvent()
    {
    }

    /**
     * @brief Returns the device ID which this event is derived from.
     */
    const int getDeviceId()
    {
        return deviceId_;
    }

    /**
     * @brief Determines whether the device ID assigned to this event is valid.
     */
    bool isDeviceIdValid()
    {
        return (deviceId_ != kInvalidDeviceId);
    }

    /**
     * @brief Sets the device ID to this event.
     * @param deviceId The ID to set.
     */
    void setDeviceId(const int deviceId)
    {
        deviceId_ = deviceId;
    }

    /**
     * @brief Sets the action of this event.
     * @param action The action to set.
     */
    void setAction(const Action action)
    {
        action_ = action;
    }

    /**
     * @brief Returns the action.
     */
    Action getAction() const
    {
        return action_;
    }

    /**
     * @brief Returns the ID assgined by a high-speed vision sensor.
     */
    unsigned char getTrackingId() const
    {
        return trackingId_;
    }

    HsvSensor::SensingState getSensingState() const
    {
        return sensingState_;
    }

    /**
     * @brief Returns the x-coordinate value.
     */
    float getX() const
    {
        return x_;
    }

    /**
     * @brief Sets the x-coordinate value to this event.
     * @param x The x-coordinate value.
     */
    void setX(const float x)
    {
        x_ = x;
    }

    /**
     * @brief Returns the y-coordinate value.
     */
    float getY() const
    {
        return y_;
    }

    /**
     * @brief Sets the y-coordinate value to this event.
     * @param y The y-coordinate value.
     */
    void setY(const float y)
    {
        y_ = y;
    }

    /**
     * @brief Returns the timestamp of this event.
     *        Note that this value is assigned by this computer.
     */
    int64_t getTimeMicros() const
    {
        return timeMicros_;
    }

private:
    // TODO: Add a variable to represent a source device.
    int deviceId_;
    Action action_;
    // tracking state
    HsvSensor::SensingState sensingState_;

    // The ID assigned by a high-speed vision sensor.
    const unsigned char trackingId_;
    // The x-coordinate value.
    float x_;
    // The y-coordinate value.
    float y_;
    // The timestamp of this event. Note that this value is assigned by this computer.
    const int64_t timeMicros_;
};



class PenColorEvent : public DoodleEvent
{
public:
    PenColorEvent(const int deviceId, const Color& color) :
        DoodleEvent(Type::PEN_COLOR),
        deviceId_(deviceId), color_(color)
    {
    }

    virtual ~PenColorEvent()
    {
    }

    /**
     * @brief Returns the device ID which this event is derived from.
     */
    const int getDeviceId()
    {
        return deviceId_;
    }

    /**
     * @brief Returns the color sent from a pen device.
     */
    Color getColor() const
    {
        return color_;
    }

private:
    // The device ID.
    const int deviceId_;
    // The color sent from a pen device.
    const Color color_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenEvent_H_
