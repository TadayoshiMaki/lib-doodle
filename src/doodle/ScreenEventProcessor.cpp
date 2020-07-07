#include "doodle/ScreenEventProcessor.h"

#include "doodle/ScreenEvent.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

ScreenEventProcessor::ScreenEventProcessor(const std::shared_ptr<ScreenEventDispatcher>& dispatcher) :
    dispatcher_(dispatcher), started_(false), exitLoop_(false)
{
}

ScreenEventProcessor::~ScreenEventProcessor()
{
}

void ScreenEventProcessor::start()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (started_.load())
    {
        return;
    }

    processorThread_ = std::thread([this] { loop(); });
}

void ScreenEventProcessor::stop()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (!started_.load())
    {
        return;
    }

    exitLoop_.store(true);
    queueCondition_.notify_one();

    if (processorThread_.joinable())
    {
        processorThread_.join();
    }
}

bool ScreenEventProcessor::isStarted()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    return started_.load();
}

void ScreenEventProcessor::handleScreensInitialized(std::vector<std::shared_ptr<ScreenProperty>> screens)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    auto initEvent = std::make_shared<ScreenInitEvent>(screens);
    queue_.push(std::move(initEvent));
    queueCondition_.notify_one();
}

void ScreenEventProcessor::handleScreenChangeStarted(const int oldScreenId, const int newScreenId)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    auto changeEvent = std::make_shared<ScreenChangeEvent>(ScreenChangeEvent::Action::START,
                                                           oldScreenId,
                                                           newScreenId);
    queue_.push(std::move(changeEvent));
    queueCondition_.notify_one();
}

void ScreenEventProcessor::handleCurrentScreenChanged(const int oldScreenId, const int newScreenId)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    auto changeEvent = std::make_shared<ScreenChangeEvent>(ScreenChangeEvent::Action::COMPLETE,
                                                           oldScreenId,
                                                           newScreenId);
    queue_.push(std::move(changeEvent));
    queueCondition_.notify_one();
}

void ScreenEventProcessor::loop()
{
    started_.store(true);
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            while (queue_.empty())
            {
                if (exitLoop_.load())
                {
                    break;
                }
                queueCondition_.wait(lock);
            }

            if (exitLoop_.load())
            {
                break;
            }

            if (queue_.empty())
            {
                continue;
            }

            auto& e = queue_.front();
            switch (e->getType())
            {
            case DoodleEvent::Type::SCREEN_INIT:
            {
                auto initEvent = std::dynamic_pointer_cast<ScreenInitEvent>(e);
                dispatcher_->notifyScreenInit(initEvent);
                break;
            }
            case DoodleEvent::Type::SCREEN_CHANGE:
            {
                auto changeEvent = std::dynamic_pointer_cast<ScreenChangeEvent>(e);
                dispatcher_->notifyChangeCurrentScreen(changeEvent);
                break;
            }
            default:
                break;
            }
            queue_.pop();
        }
    }
    started_.store(false);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
