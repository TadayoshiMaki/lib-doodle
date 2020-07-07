#ifndef LIB_DOODLE_FRAMEWORK_doodle_HsvSensor_H_
#define LIB_DOODLE_FRAMEWORK_doodle_HsvSensor_H_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <opencv2/core/core.hpp>

#include "doodle/namespace.h"
#include "doodle/CameraUtils.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

/**
 * @brief An abstract high-speed vision sensor class.
 */
class HsvSensor
{
public:
    enum class SensingState {
        DETECT = 0,
        TRACK,
        LOST,
        NONE
    };
    struct SensingResult
    {
        SensingState sensingState;
        uint8_t trackingId;
        float x;
        float y;
        int64_t timeMicros;

        SensingResult(SensingState state, uint8_t trackingId, float x, float y, int64_t timeMicros):
            sensingState(state), trackingId(trackingId), x(x), y(y), timeMicros(timeMicros)
        {
        }
    };

    enum class Model
    {
        kUnknownVendor_UnknowSensor,
        kSony_EvaluationHsvSensor,
        kOmronSentech_HsvChipSensor,
        kOmronSentech_HsvChipSensor_MultiTrack,

    };

    enum class PixelFormat
    {
        RGB,
        BGR,
        GRAY
    };

    /**
     * @brief An abstract sensor configuration class.
     */
    class Config
    {
    public:
        // A pure virtual destructor for abstraction.
        virtual ~Config() = 0 {}
    };

    using CaptureCallback = std::function<void(const std::shared_ptr<cv::Mat>&)>;
    using SensingResultCallback = std::function<void(const std::vector<SensingResult>&)>;

    HsvSensor() : opened_(false), started_(false), captureEnabled_(false), fps_(kDefaultFps)
    {
        frame_ = std::make_shared<cv::Mat>();
        setCallbackIntervalFps(fps_);
    }

    virtual ~HsvSensor()
    {
        finalizeSensor();
    }

    /**
     * @brief Opens a high-speed vision sensor with the configuration.
     * @param config The sensor configuration to use.
     * @return true if successfully opened.
     */
    bool open(const std::shared_ptr<Config>& config)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // Already opened.
        if (opened_) return true;
        opened_ = onOpenSensor(config);
        return opened_;
    }

    /**
     * @brief Closes a high-speed vision sensor.
     */
    void close()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        // Stop the sensor if already started.
        if (started_) stop();
        onCloseSensor();
        opened_ = false;
    }

    /**
     * @brief Determines whether a high-speed vision sensor is opened.
     * @return true if a sensor is opened.
     */
    bool isOpened()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return opened_;
    }

    /**
     * @brief Sets an undistorter used to undistort detected points.
     */
    void setUndistorter(const std::shared_ptr<Undistorter>& undistorter)
    {
        undistorter_ = undistorter;
    }

    /**
     * @brief Returns an undistorter which is previously set.
     */
    std::shared_ptr<Undistorter> getUndistorter()
    {
        return undistorter_;
    }

    /**
     * @brief Sets whether frame capturing is enabled.
     * @param enable true to set this sensor to use frame capturing, false otherwise.
     */
    void setCaptureEnabled(bool enabled)
    {
        captureEnabled_ = enabled;
    }

    /**
     * @brief Returns whether the frame capturing is enabled or not.
     */
    bool isCaptureEnabled()
    {
        return captureEnabled_;
    }

    /**
     * @brief Sets the callback interval as the frame per second.
     * @param fps The frame per second.
     */
    void setFps(const uint16_t fps)
    {
        fps_ = fps;
        setCallbackIntervalFps(fps_);
    }

    /**
     * @brief Returns the callback interval as the frame per second.
     */
    uint16_t getFps()
    {
        return fps_;
    }

    /**
     * @brief Starts capturing frames and color detection.
     * @return true if successfully started.
     */
    bool start()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_ || started_) return false;

        started_ = onStartSensor();
        if (started_)
        {
            capturingThread_ = std::thread(&HsvSensor::run, this);
        }
        return started_;
    }

    /**
     * @brief Stops capturing frames.
     * @return true if successfully stopped.
     */
    virtual bool stop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_ || !started_) return false;

        started_ = !onStopSensor();
        if (started_) return false;

        if (capturingThread_.joinable())
        {
            capturingThread_.join();
        }
        return true;
    }

    /**
     * @brief Determines whether frame capturing is started or not.
     * @return true if frame capturing is started.
     */
    bool isStarted()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return started_;
    }

    /**
     * @brief Returns the width of captured frames.
     * @return The width in pixels.
     */
    virtual uint32_t getWidth() = 0;
    /**
     * @brief Returns the height of captured frames.
     * @return The height in pixels.
     */
    virtual uint32_t getHeight() = 0;

    /**
     * @brief Returns the pixel format of captured frames.
     * @return The pixel format.
     */
    virtual PixelFormat getPixelFormat() = 0;

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

    void setCaptureCallback(const CaptureCallback& captureCallback)
    {
        captureCallback_ = std::make_shared<CaptureCallback>(captureCallback);
    }
    /**
     * @brief Sets the callback to subscribe the detected points by the high-speed vision sensor.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method with the arguments.
     */
    template<class ListenerClass>
    void setDetectionResultCallback(ListenerClass* owner,
        void (ListenerClass::*listenerMethod)(const std::vector<SensingResult>&))
    {
        setDetectionResultCallback(std::bind(listenerMethod, owner, std::placeholders::_1));
    }

    void setDetectionResultCallback(const SensingResultCallback& sensingResultCallback)
    {
        sensingResultCallback_ = std::make_shared<SensingResultCallback>(sensingResultCallback);
    }

    /**
     * @brief Returns the current sensing results.
     */
    std::vector<SensingResult> getCurrentSensingResults()
    {
        return results_;
    }
    /**
     * @brief Returns the current captured frame.
     */
    std::shared_ptr<cv::Mat> getCurrentFrame()
    {
        return frame_;
    }

    void setUseRegionNum(int num) 
    {
        if (num < 1 || num > kMaxRegionNum) return;
        useRegionNum_ = num;
    }

protected:
    // The worker thread to process frames.
    std::thread capturingThread_;
    // The lock object for capturing frames.
    std::mutex mutex_;

    // The flag indicates whether the camera device is opened.
    std::atomic<bool> opened_;
    // The flag indicates whether the frame capturing is started.
    std::atomic<bool> started_;
    // The flag indicates whether the frame capturing is enabled.
    std::atomic<bool> captureEnabled_;

    // Used to undistort detected points.
    std::shared_ptr<Undistorter> undistorter_;
    // The frame per second of a sensor.
    uint16_t fps_;
    // The capturing callback interval in microseconds.
    uint64_t callbackIntervalMicros_;

    // The current captured frame.
    std::shared_ptr<cv::Mat> frame_;
    // The list of current sensing results.
    std::vector<SensingResult> results_;

    // The callable object to notify of captured frames.
    std::shared_ptr<CaptureCallback> captureCallback_;
    // The callable object to notify of sensing results.
    std::shared_ptr<SensingResultCallback> sensingResultCallback_;

    int useRegionNum_ = 1;

    /**
     * @brief Called to open a high-speed vision sensor.
     * @return true if successfully opened.
     */
    virtual bool onOpenSensor(const std::shared_ptr<Config>& config) = 0;

    /**
     * @brief Called to close a high-speed vision sensor.
     */
    virtual void onCloseSensor() = 0;

    /**
     * @brief Called to start a high-speed vision sensor.
     * @return true if successfully started.
     */
    virtual bool onStartSensor() = 0;
    /**
     * @brief Called to stop a high-speed vision sensor.
     * @return true if successfully stopped.
     */
    virtual bool onStopSensor() = 0;

    /**
     * @brief Captures frames in a loop on the worker thread.
     */
    virtual void run() = 0;

    /**
     * @brief Assigns a state of sensing result
     * @param curResults current hsv sensing result (not assign sensing state.)
     * @param preResults previous hsv sensing result (already assigned sensing state.)
     * @param currentTimeMicros current time in micro second.
     
     */
    virtual void assignSensingState(std::vector<SensingResult>& curResults, const std::vector<SensingResult>& preResults, int64_t currentTimeMicros);
private:
    // The default frame rate.
    static constexpr int kDefaultFps = 60;
    // The Max point detect num
    static constexpr int kMaxRegionNum = 6;


    /**
     * @brief Sets a callback interval from the frame per second.
     * @param fps The frame per second.
     */
    void setCallbackIntervalFps(const uint16_t fps)
    {
        std::chrono::microseconds time = std::chrono::seconds(1);
        callbackIntervalMicros_ = time.count() / fps;
    }

    /**
     * @brief Releases resources related to a sensor.
     */
    void finalizeSensor()
    {
        if (started_) stop();
        if (opened_) close();
        frame_->release();
        results_.clear();
    }

};

/**
 * TODO: Create a map in advance.
 */
class HsvSensorFactory
{
public:
    /**
     * @brief Returns a model from a name.
     * @param name The model name.
     */
    static HsvSensor::Model getModel(const std::string& name);
    /**
     * @brief Creates a high-speed sensor instance.
     * @param model The sensor model to create.
     */
    static std::shared_ptr<HsvSensor> create(const HsvSensor::Model model);
private:
    HsvSensorFactory() = delete;
    HsvSensorFactory(const HsvSensorFactory&) = delete;
    HsvSensorFactory& operator=(const HsvSensorFactory&) = delete;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_HsvSensor_H_
