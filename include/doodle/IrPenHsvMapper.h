#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenHsvMapper_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenHsvMapper_H_

#include <map>
#include <memory>
#include <deque>

#include "doodle/namespace.h"
#include "doodle/IrPenState.h"
#include "doodle/IrPenSearchTimer.h"
#include "doodle/IrPenManager.h"
#include "doodle/HsvSensor.h"
#include "doodle/IrPenEvent.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPenHsvMapper : public IrPenSearchTimerListener {
public:
    //struct PenHsv {
    //    int hsvId;
    //    bool isTipPressed;
    //    bool irLedState;
    //    PenHsv(int hsvId= kInvalidId, bool isTipPressed = false) :
    //        hsvId(hsvId), 
    //        isTipPressed(isTipPressed),
    //        irLedState(false)
    //    {
    //    }
    //    void reset() {
    //        hsvId = kInvalidId;
    //        isTipPressed = false;
    //    }
    //};

    struct PenHsvSearch
    {
        enum SearchState{
            kWait,     //waiting 
            kIrLedOn,  //From the command to turn on the IR LED to completion
            kIrLedOff, //From the command to turn off the IR LED to completion. This state is when lost or timeout happened.
            kSearch,   //Completed IR LED On and hsv sensor searching
        };
        int penId;
        SearchState state;
        PenHsvSearch(const int penId, const SearchState state = kWait) :
            penId(penId),
            state(state)
        {
        }
    };
    

    /**
     * @brief Sets the callback to subscribe the captured frames by the high-speed vision sensor.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method with the arguments.
     */
    template<class ListenerClass>
    void setCaptureCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const std::shared_ptr<cv::Mat>&))
    {
        setFrameCallback(std::bind(listenerMethod, owner, std::placeholders::_1));
    }

    void setCaptureCallback(const HsvSensor::CaptureCallback& captureCallback)
    {
        captureCallback_ = std::make_shared<HsvSensor::CaptureCallback>(captureCallback);
    }

    /**
     * @brief Sets the callback to subscribe the detected points by the high-speed vision sensor.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method with the arguments.
     */
    template<class ListenerClass>
    void setDetectionResultCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const std::vector<HsvSensor::SensingResult>&))
    {
        setDetectionResultCallback(std::bind(listenerMethod, owner, std::placeholders::_1));
    }

    void setDetectionResultCallback(const HsvSensor::SensingResultCallback& sensingResultCallback)
    {
        sensingResultCallback_ = std::make_shared<HsvSensor::SensingResultCallback>(sensingResultCallback);
    }


    IrPenHsvMapper(const std::shared_ptr<IrPenManager>& penManager,
        const std::shared_ptr<HsvSensor>& strokeDetector);
    ~IrPenHsvMapper();

    /**
     * @brief Starts processing event data.
     * @return true if successfully started.
     */
    void start();
    /**
     * @brief Stops processing event data.
     * @return true if successfully stopped.
     */
    void stop();
    /**
     * @brief Determines whether processing is started or not.
     * @return true if reading is started.
     */
    bool isStarted();

    /**
     * @brief Processes received data in a loop.
     */
    void run();

    /**
     * @brief Called when points are detected by a sensor.
     * @param results The detection results
     */
    void onStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results);
    /**
     * @brief Called when the IR led state order is completed.
     * @param deviceId The device ID of a pen.
     * @param state The IR led state.
     */
    void onIrLedStateChanged(const int deviceId, const bool state);

    /**
     */
    void onPenSearchTimeout(const int penId, const bool cancelled) override;

    void devicesUpdate(const std::shared_ptr<PenDeviceUpdateEvent> e);
private:
    bool isReadySearch();
    bool isReadyDetect();
    void mapPenHsv(const int deviceId, const int hsvId);
    void unmapPenHsv(const int deviceId);
    bool isAlreadyMapped(const int hsvId);
    int getMappedPenId(const int hsvId);
private:
    static const int kDefaultRequestTimeoutMillis = 500;

    const static int kInvalidId = -1;
    std::map<int, int> penHsvMap_; // <penId, hsvId>
    std::deque<PenHsvSearch> searchQue_;

    // Used to ir pen search timer
    std::shared_ptr<IrPenSearchTimer> penSearchTimer_;
    // Used to ir pen search timer
    std::shared_ptr<IrPenManager> penManager_;
    // Used to detect ir point
    std::shared_ptr<HsvSensor> strokeDetector_;

    // The callable object to notify of captured frames.
    std::shared_ptr<HsvSensor::CaptureCallback> captureCallback_;
    // The callable object to notify of sensing results.
    std::shared_ptr<HsvSensor::SensingResultCallback> sensingResultCallback_;

    // The worker thread to process event data.
    std::thread processorThread_;
    // The condition for the event queue.
    std::condition_variable queueCondition_;
    // The lock object for thread operations.
    std::mutex processorMutex_;
    // The flag indicating whether the processing loop is started or not.
    std::atomic_bool started_;
    // The flag indicating whether the loop should exit or not.
    std::atomic_bool exitLoop_;

    // The lock object for thread operations.
    std::mutex mutex_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenHsvMapper_H_
