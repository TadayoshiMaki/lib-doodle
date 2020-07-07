#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPen_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPen_H_

#include <memory>
#include <vector>
#include <functional>

#include "doodle/namespace.h"
#include "doodle/IrPenListener.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPen
{
public:
    // IR pen service UUID.
    static const std::string kServiceUuid;
    // RGB LED config characteristic UUID.
    static const std::string kRgbLedConfigCharacteristicUuid;
    // RGB LED color characteristic UUID.
    static const std::string kRgbLedColorCharacteristicUuid;
    // IR LED config characteristic UUID.
    static const std::string kIrLedConfigCharacteristicUuid;
    // Pen tip state characteristic UUID.
    static const std::string kTipStateCharacteristicUuid;
    // Color picker characteristic UUID.
    static const std::string kColorPickerCharacteristicUuid;

    IrPen();
    virtual ~IrPen();

    void addListener(const std::shared_ptr<IrPenListener>& l)
    {
        auto& it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const std::shared_ptr<IrPenListener>& l)
    {
        auto& it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it != listeners_.end())
        {
            listeners_.erase(it);
        }
    };

    /**
     * @brief Resumes the pen device. The pen will start to notify of or indicate characteristic value changes.
     */
    virtual bool resume() = 0;

    /**
     * @brief Pauses the pen device. The pen will stop to notify of or indicate characteristic value changes.
     */
    virtual void pause() = 0;

    /**
     * @brief Determines whether the pen device is resumed or not.
     */
    virtual bool isResumed() = 0;

protected:
    /**
     * @brief Subscribes server messages (notification and indication).
     * @return true if subscription is successfully started.
     */
    virtual bool subscribeServerMessages() = 0;
    /**
     * @brief Unsubscribes server messages (notification and indication).
     */
    virtual void unsubscribeServerMessages() = 0;

    /**
     * @brief Called when the pen device is resumed.
     */
    virtual void onResume() = 0;

    /**
     * @brief Called when the pen device is paused.
     */
    virtual void onPause() = 0;

    /**
     * @brief Dispatches resume state notification of the pen device.
     */
    virtual void dispatchResume() = 0;

    /**
     * @brief Dispatches pause state notification of the pen device.
     */
    virtual void dispatchPause() = 0;

    void foreachListeners(std::function<void(std::shared_ptr<IrPenListener>)>);

private:
    std::vector<std::shared_ptr<IrPenListener>> listeners_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPen_H_
