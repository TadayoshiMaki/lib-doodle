#include "doodle/IrPenHsvMapper.h"
#include "HsvSensorProdMultitrack.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

IrPenHsvMapper::IrPenHsvMapper( const std::shared_ptr<IrPenManager>& penManager,
                                const std::shared_ptr<HsvSensor>& strokeDetector) :
    penManager_(penManager),
    strokeDetector_(strokeDetector),
    penSearchTimer_(std::make_shared<IrPenSearchTimer>(kDefaultRequestTimeoutMillis))
{
    //set callback functions
    strokeDetector_->setDetectionResultCallback(this, &IrPenHsvMapper::onStrokePointDetected);
    penManager_->setIrLedStateChangedCallback(this, &IrPenHsvMapper::onIrLedStateChanged);
    penSearchTimer_->addListener(this);
}
IrPenHsvMapper::~IrPenHsvMapper()
{
    stop();
    penSearchTimer_->removeListener(this);
    penManager_.reset();
    strokeDetector_.reset();
    penSearchTimer_.reset();
}

void IrPenHsvMapper::start()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (started_) return;
    processorThread_ = std::thread([this] { run(); });
}

void IrPenHsvMapper::stop()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (!started_) return;
    exitLoop_ = true;

    if (processorThread_.joinable())
    {
        processorThread_.join();
    }
}

bool IrPenHsvMapper::isStarted()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    return started_;
}

void IrPenHsvMapper::run()
{
    started_ = true;
    while (true)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (isReadySearch())
        {
            auto &que = searchQue_.front();
            penManager_->setIrLedState(que.penId, true);
            penSearchTimer_->startTimeout(que.penId);
            que.state = PenHsvSearch::kIrLedOn;
        }
        if (exitLoop_) break;
    }
    started_ = false;

}

void IrPenHsvMapper::onStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<HsvSensor::SensingResult> editResults;
    for (const auto &r : results)
    {
        HsvSensor::SensingResult editResult(r);
        switch (r.sensingState)
        {
        case HsvSensor::SensingState::DETECT:
            if (isReadyDetect())
            {
                // map
                int deviceId = searchQue_.front().penId;
                mapPenHsv(deviceId, (int)r.trackingId);
                editResult.trackingId = deviceId;
            }
            break;
        case HsvSensor::SensingState::TRACK:
            if (isAlreadyMapped((int)r.trackingId)) {
                editResult.trackingId = getMappedPenId((int)r.trackingId);
                break; 
            }
            if (isReadyDetect())
            {
                // map
                int deviceId = searchQue_.front().penId;
                mapPenHsv(deviceId, (int)r.trackingId);
                editResult.trackingId = deviceId;
            }
            break;
        case HsvSensor::SensingState::LOST:
            for (auto& m : penHsvMap_) {
                if (m.second == (int)r.trackingId) { //find lost pen id
                    //unmap
                    int deviceId = m.first;
                    unmapPenHsv(deviceId);
                    editResult.trackingId = deviceId;
                    break;
                }
            }
            break;
        case HsvSensor::SensingState::NONE:
            break;
        }
        //edit result id. from hsv id to mapped pen Id
        editResults.emplace_back(editResult);
    }
    if (sensingResultCallback_) { (*sensingResultCallback_)(editResults); }
}

void IrPenHsvMapper::onIrLedStateChanged(const int deviceId, const bool state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (penHsvMap_.find(deviceId) == penHsvMap_.end()) { return; }
    for(auto &q : searchQue_)
    {
        if (q.penId == deviceId) {
            if (state) { 
                auto s = std::dynamic_pointer_cast<SentechHsvSensorMultiTrack>(strokeDetector_);
                s->setCanFindNewPoint(true);
                q.state = PenHsvSearch::kSearch;
            }
            else { q.state = PenHsvSearch::kWait; }
            break;
        }
    }
}

void IrPenHsvMapper::onPenSearchTimeout(const int penId, const bool cancelled)
{
    std::lock_guard<std::mutex> lock(mutex_);
    //time out searching ir led will be off.
    penManager_->setIrLedState(penId, false);
    searchQue_.pop_front();
    searchQue_.emplace_back(penId, PenHsvSearch::kIrLedOff);
}

void IrPenHsvMapper::devicesUpdate(const std::shared_ptr<PenDeviceUpdateEvent> e)
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Initialize the device Ir Led state.
    const size_t count = e->getDeviceCount();
    for (int i = 0; i < count; i++)
    {
        IrPenDeviceStatus s = e->getDeviceStatus(i);
        bool ret = penManager_->setIrLedState(s.deviceId, false);
        ret = ret && penManager_->setRgbLedState(s.deviceId, RgbLedState::kBlink, s.rgbLedColor);
        if (!ret) {
            std::cerr << "pen Manager error" << std::endl;
        }
        penHsvMap_[s.deviceId] = kInvalidId;
        searchQue_.emplace_back(s.deviceId);
    }
    strokeDetector_->setUseRegionNum(count);
    strokeDetector_->start();
}

bool IrPenHsvMapper::isReadySearch()
{
    if (searchQue_.empty()) { return false; }
    for(const auto &q : searchQue_)
    {
        //check state
        if (q.state != PenHsvSearch::kWait) { return false; }
    }
    return true;
}

bool IrPenHsvMapper::isReadyDetect()
{
    if (searchQue_.empty()) { return false; }
    for (const auto &q : searchQue_)
    {
        //check state
        if (q.state == PenHsvSearch::kIrLedOff ||
            q.state == PenHsvSearch::kIrLedOn)
        { 
            return false;
        }
    }
    return true;
}

void IrPenHsvMapper::mapPenHsv(const int deviceId, const int hsvId)
{
    //map pen and hsv id
    penHsvMap_[deviceId] = hsvId;
    //cancel timer
    penSearchTimer_->cancelTimeout(deviceId);
    //pop queue
    searchQue_.pop_front();
    //rgb led on
    penManager_->setRgbLedState(deviceId, RgbLedState::kTurnedOn);
}

void IrPenHsvMapper::unmapPenHsv(const int deviceId)
{
    //unmap invalid id
    penHsvMap_[deviceId] = kInvalidId;
    //reset pen state
    penManager_->setIrLedState(deviceId, false);
    penManager_->setRgbLedState(deviceId, RgbLedState::kBlink);
    //add queue
    searchQue_.emplace_back(deviceId, PenHsvSearch::kIrLedOff);
}

bool IrPenHsvMapper::isAlreadyMapped(const int hsvId)
{
    for (auto& d : penHsvMap_)
    {
        if (d.second == (int)hsvId)
        {
            return true;
        }
    }
    return false;
}

int IrPenHsvMapper::getMappedPenId(const int hsvId)
{
    for (auto& m : penHsvMap_) {
        if (m.second == hsvId) {
            return m.first;
        }
    }
    return kInvalidId;
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
