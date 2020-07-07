#include "doodle/AsyncPointDetector.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

AsyncPointDetector::AsyncPointDetector(const std::shared_ptr<mpj_Camera>& camera,
                                       const uint8_t threshold) :
    camera_(camera), threshold_(threshold), started_(false)
{
}

AsyncPointDetector::~AsyncPointDetector()
{
    if (started_) stop();

    if (frameCallback_) frameCallback_.reset();
    if (detectionCallback_) detectionCallback_.reset();

    pointDetector_.reset();
}

void AsyncPointDetector::start(const FrameCallback& detectionCallback)
{
    start(detectionCallback);
}

void AsyncPointDetector::start(const DetectionCallback& detectionCallback, const FrameCallback& frameCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (started_) return;

    frameCallback_ = std::make_shared<FrameCallback>(frameCallback);
    detectionCallback_ = std::make_shared<DetectionCallback>(detectionCallback);

    camera_->startCapture();
    started_ = true;
    processorThread_ = std::thread(&AsyncPointDetector::run, this);
}

void AsyncPointDetector::stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!started_) return;

    // Stop the worker thread.
    started_ = false;
    if (processorThread_.joinable())
    {
        processorThread_.join();
    }
    camera_->stopCapture();
}

bool AsyncPointDetector::isStarted()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return started_;
}

void AsyncPointDetector::setMask(const std::string & maskFilePath)
{
    _FSPFX path filePath(maskFilePath);
    if (!_FSPFX exists(filePath)) return;
    if (pointDetector_) pointDetector_->setFrameMask(filePath);
}

void AsyncPointDetector::setZeta(const float value)
{
    if (pointDetector_) pointDetector_->setZeta(value);
}

void AsyncPointDetector::run()
{
    // Should not start processing under abnormal conditions.
    if (!camera_ || !pointDetector_) return;

    std::shared_ptr<const mpj_Image> frame;
    while (started_)
    {
        frame = camera_->captureFrameWait();
        if (!frame) continue;

        if (frameCallback_) (*frameCallback_)(frame);
        // TODO: Should load a value from a file.
        auto points = pointDetector_->detectPointsOnFrame(frame.get(), threshold_);
        if (!points.empty())
        {
            if (detectionCallback_) (*detectionCallback_)(std::move(points));
        }
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
