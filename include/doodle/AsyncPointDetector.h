#ifndef LIB_DOODLE_FRAMEWORK_doodle_AsyncPointDetector_H_
#define LIB_DOODLE_FRAMEWORK_doodle_AsyncPointDetector_H_

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include "mpj_camera.h"
#include "mpj_pointdetector.h"
#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using mpjsdk::mpj_Camera;
using mpjsdk::mpj_DetectedPoint;
using mpjsdk::mpj_Image;
using mpjsdk::mpj_PointDetector;

class AsyncPointDetector
{
public:
    using FrameCallback = std::function<void(const std::shared_ptr<const mpj_Image>&)>;
    using DetectionCallback = std::function<void(std::vector<mpj_DetectedPoint>)>;

    AsyncPointDetector(const std::shared_ptr<mpj_Camera>& camera,
                       const uint8_t threshold = kDefaultThreshold);
    virtual ~AsyncPointDetector();

    /**
     * @brief Starts the point detector with the callable object for point detection results.
     * @param detectionCallback The callable object for point detection results.
     */
    void start(const FrameCallback& detectionCallback);

    /**
     * @brief Starts the point detector with the callable objects for captured frames and point detection results.
     * @param frameCallback The callable object for captured frames.
     * @param detectionCallback The callable object for point detection results.
     */
    void start(const DetectionCallback& detectionCallback, const FrameCallback& frameCallback = nullptr);

    /**
     * @brief Stops the point detector.
     */
    void stop();

    /**
     * @brief Determines whether the point detector is started or not.
     * @return true if the point detector is started.
     */
    bool isStarted();

    /**
     * @brief Sets the mask for point detection.
     * @param maskFilePath The path of a mask file.
     */
    void setMask(const std::string& maskFilePath);

    /**
     * @brief Sets the brightness value of noises including salt-and-pepper noise
     *        which depends on only the gain parameter of a camera.
     * @param value The brightness value of noises.
     */
    void setZeta(const float value);

private:
    // The default threshold to create binary images.
    // TODO: Read out from a file.
    static const uint8_t kDefaultThreshold = 128;

    // The worker thread to process frames.
    std::thread processorThread_;
    // The lock object for point detection.
    std::mutex mutex_;
    // The flag indicates whether the point detector is started.
    std::atomic<bool> started_;

    // The camera to capture frames.
    std::shared_ptr<mpj_Camera> camera_;
    // The point detector used on the worker thread.
    std::shared_ptr<mpj_PointDetector> pointDetector_;
    // The threshold used to binarize images.
    uint8_t threshold_;

    // The callable object to notify captured frames.
    std::shared_ptr<FrameCallback> frameCallback_;
    // The callable object to notify point detection results.
    std::shared_ptr<DetectionCallback> detectionCallback_;

    /**
     * @brief Captures frames and detects points in them in a loop on the worker thread.
     */
    void run();
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_AsyncPointDetector_H_
