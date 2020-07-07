#include "HsvSensorProdMultitrack.h"
#include <opencv2/opencv.hpp>

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

SentechHsvSensorMultiTrack::SentechHsvSensorMultiTrack() : SentechHsvSensor()
{
}

SentechHsvSensorMultiTrack::~SentechHsvSensorMultiTrack()
{

}

void SentechHsvSensorMultiTrack::setCaptureEnabled(bool enabled)
{
    CNodeMapPtr nodeMap(device_->GetRemoteIStPort()->GetINodeMap());
    setValue<int>(nodeMap, "ChunkSelector", 0);
    setValue<bool>(nodeMap, "ChunkEnable", enabled);
    captureEnabled_ = enabled;
}

void SentechHsvSensorMultiTrack::setCanFindNewPoint(bool value)
{
    canFindingNewPoint_ = value;
}

void SentechHsvSensorMultiTrack::run()
{
    setCaptureEnabled(true);
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::system_clock;
    using GenApi::CNodeMapPtr;
    using StApi::CIStImageBufferPtr;
    using StApi::EStTimeoutHandling_t;

    auto lastFrameTime = system_clock::now();
    auto currentTime = system_clock::now();

    CNodeMapPtr nodeMap(device_->GetRemoteIStPort()->GetINodeMap());
    std::vector<int> regionIds(useRegionNum_ + 1); //plus one window is used for search new point
    //Initialize each region detect settings
    for (int i = 0; i < regionIds.size(); i++)
    {
        regionIds[i] = i;
        setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", i);
        setValue<int>(nodeMap, "ObjectTrackingFrameMode", kObjectTrackingFrameModeManual);
        setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
#ifdef DEBUG_CAMIMAGE_
        setValue<bool>(nodeMap, "ObjectDetectionFrameDisplayEnable", true);
#else
        setValue<bool>(nodeMap, "ObjectDetectionFrameDisplayEnable", false);
#endif
        if (i != regionIds.size() - 1)
        {
            autoObjectDetectManager_.emplace_back(i, kWaiting);
        }
    }
    SEARCHWINDOWID = regionIds.size() - 1;
    std::vector<SensingResult> prevResults;
    while (started_) {
        if (!stream_->IsGrabbing()) continue;
        CIStStreamBufferPtr buffer(
            stream_->RetrieveBuffer(1, EStTimeoutHandling_t::StTimeoutHandling_Count));
        if (!buffer) continue;

        auto imageAvailable = buffer->GetIStStreamBufferInfo()->IsImagePresent();
        if (imageAvailable && captureEnabled_) {
            onAcquireImage(buffer);
#ifdef DEBUG_CAMIMAGE_
            cv::imshow("test", *frame_);
            cv::waitKey(1);
#endif
        }
        currentTime = system_clock::now();
        auto currentTimeEpoch = currentTime.time_since_epoch();
        int64_t currentTimeMicros = duration_cast<microseconds>(currentTimeEpoch).count();

        std::map<int, hsvResult> results;
        for (auto&& regionId : regionIds)
        {
            bool selected = setValue(nodeMap, "ChunkDetectObjectRegionSelector", regionId);
            if (!selected) continue;
            bool detected = getValue<bool>(nodeMap, "ChunkDetectObjectResult");
            if (!detected) continue;
            int moment0 = getValue<int>(nodeMap, "ChunkDetectObjectMoment0");
            int moment1X = getValue<int>(nodeMap, "ChunkDetectObjectMoment1X");
            int moment1Y = getValue<int>(nodeMap, "ChunkDetectObjectMoment1Y");
            PointF point((float)moment1X / moment0, (float)moment1Y / moment0);
            hsvResult res(
                detected,
                static_cast<unsigned char>(regionId),
                point.x,
                point.y,
                currentTimeMicros,
                moment0,
                moment1X,
                moment1Y);
            results.insert(std::pair<int, hsvResult>(res.trackingId, res));
        }
        hsvResultMap_ = results;
        //multi object tracking impelement
        for (auto& a : autoObjectDetectManager_)
        {
            if (!canFindingNewPoint_ && a.objectSearchState != kRegistAreaTrack)
            {
                setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                setValue<int>(nodeMap, "ObjectDetectionFrameCenterPositionMode", kObjectDetectionFrameCenterPositionModeRegister);
                setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
                a.objectSearchState = kLost;
            }

            switch (a.objectSearchState)
            {
            case kWaiting:
                if (isReadySearch(a.id) && canFindingNewPoint_)
                {
                    //start searching new point
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", SEARCHWINDOWID);
                    setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeAll);
                    a.objectSearchState = kAllAreaSearch;
                    a.stableCounter = 0;
                }
                break;
            case kAllAreaSearch:
            {
                if (hsvResultMap_.find(SEARCHWINDOWID) == hsvResultMap_.end()) { break; }

                //check find new point
                float newPointX, newPointY;
                if (findNewPoint(a.id, newPointX, newPointY))
                {
                    //set new point detection area
                    int searchW = kSearchAreaSize_, searchH = kSearchAreaSize_;
                    int searchCentX = newPointX, searchCentY = newPointY;
                    if (newPointX + searchW > getWidth())  searchCentX = getWidth() - searchW / 2;
                    if (newPointX - searchW < 0)  searchCentX = searchW / 2;
                    if (newPointY + searchH > getHeight())  searchCentY = getHeight() - searchH / 2;
                    if (newPointY - searchH < 0)  searchCentY = searchH / 2;
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                    setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeRegister);
                    setValue<int>(nodeMap, "ObjectDetectionFrameCenterX", searchCentX);
                    setValue<int>(nodeMap, "ObjectDetectionFrameCenterY", searchCentY);
                    setValue<int>(nodeMap, "ObjectDetectionFrameWidth", searchW);
                    setValue<int>(nodeMap, "ObjectDetectionFrameHeight", searchH);
                    a.searchX = searchCentX, a.searchY = searchCentY;
                    a.searchW = searchW, a.searchH = searchH;
                    a.objectSearchState = kRegistAreaSearch;

                    //stop searching new point
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", SEARCHWINDOWID);
                    setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
                }
                break;
            }
            case kRegistAreaSearch:
                //wait for detecting registered area
                if (hsvResultMap_.find(a.id) == hsvResultMap_.end()) {
                    if (a.stableCounter++ > kRregisterDetectionWaitFrameCountThre_) {
                        //failure find new point
                        setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                        setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
                        a.objectSearchState = kLost;
                        canFindingNewPoint_ = false;
                    }
                    break;
                }

                // check new point pos.
                if (isPointInArea(hsvResultMap_[a.id].x, hsvResultMap_[a.id].y, a.searchX, a.searchY, a.searchW, a.searchH))
                {
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                    setValue<int>(nodeMap, "ObjectDetectionFrameCenterPositionMode", kObjectDetectionFrameCenterPositionModeGravityPosition);
                    setValue<int>(nodeMap, "ObjectDetectionFrameWidth", kTrackingAreaSize_);
                    setValue<int>(nodeMap, "ObjectDetectionFrameHeight", kTrackingAreaSize_);
                    a.objectSearchState = kRegistAreaDetect;
                }
                break;
            case kRegistAreaDetect:
                if (hsvResultMap_.find(a.id) == hsvResultMap_.end()) { //lost happened
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                    setValue<int>(nodeMap, "ObjectDetectionFrameCenterPositionMode", kObjectDetectionFrameCenterPositionModeRegister);
                    setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
                    a.objectSearchState = kLost;
                    canFindingNewPoint_ = false;
                    break;
                }
                a.objectSearchState = kRegistAreaTrack;
                break;
            case kRegistAreaTrack:
                if (hsvResultMap_.find(a.id) == hsvResultMap_.end() || exsistSamePosPoint(a.id)) { //lost happened
                    setValue<int>(nodeMap, "ObjectDetectionFrameRegionSelector", a.id);
                    setValue<int>(nodeMap, "ObjectDetectionFrameCenterPositionMode", kObjectDetectionFrameCenterPositionModeRegister);
                    setValue<int>(nodeMap, "ObjectDetectionFrameSize", kObjectDetectionFrameSizeOff);
                    a.objectSearchState = kLost;
                    canFindingNewPoint_ = false;
                }                
                break;
            case kLost:
                a.objectSearchState = kWaiting;
                break;
            case kUnknown:
            default:
                break;
            }
        }

        //remove non tracking state results
        for (auto& a : autoObjectDetectManager_) {
            if (a.objectSearchState != kRegistAreaTrack && hsvResultMap_.find(a.id) != hsvResultMap_.end()) {
                hsvResultMap_.erase(a.id);
            }
        }
        //remove search window id results
        if (hsvResultMap_.find(SEARCHWINDOWID) != hsvResultMap_.end()) {
            hsvResultMap_.erase(SEARCHWINDOWID);
        }

        //convert detect point coordinate
        std::vector<SensingResult> tempResults;
        for (auto& res : hsvResultMap_)
        {
            PointF dstPoint(res.second.x, res.second.y);
            if (undistorter_)
            {
                dstPoint = *(undistorter_->undistort(PointF(res.second.x, res.second.y)));
            }

            tempResults.emplace_back(SensingState::NONE, static_cast<unsigned char>(res.second.trackingId),
                dstPoint.x,
                dstPoint.y,
                currentTimeMicros);
        }

        assignSensingState(tempResults, prevResults, currentTimeMicros);
        results_ = std::move(tempResults);
        prevResults = results_;

        //if results are empty, then continue.
        if (results_.empty()) { continue; }
        bool shouldCallbackNow = false;
        for (auto& res : results_) {
            if (res.sensingState == SensingState::DETECT || res.sensingState == SensingState::LOST) {
                shouldCallbackNow = true;
                break;
            }
        }

        auto elapsedMicros = duration_cast<microseconds>(currentTime - lastFrameTime).count();
        //callback
        if (shouldCallbackNow)
        {
            if (sensingResultCallback_) { (*sensingResultCallback_)(results_); }
            lastFrameTime = currentTime;
        }
        else if( elapsedMicros > callbackIntervalMicros_)
        {
            lastFrameTime = currentTime;
            if (sensingResultCallback_) { (*sensingResultCallback_)(results_); }
            if (captureEnabled_ && captureCallback_) { (*captureCallback_)(frame_); }
        }
    }
}

bool SentechHsvSensorMultiTrack::findNewPoint(int windowId, float& x, float& y)
{
    //error check
    if (hsvResultMap_.find(SEARCHWINDOWID) == hsvResultMap_.end()) { return false; }
    //check other windows state are NOT tracking
    bool isOtherTracking = false;
    for (auto& a : autoObjectDetectManager_)
    {
        if (a.id != windowId && a.id != SEARCHWINDOWID && a.objectSearchState == kRegistAreaTrack)
        {
            isOtherTracking = true;
        }
    }

    //find new point if other window state is waiting or lost
    if (!isOtherTracking)
    {
        //find new point centerX, centerY
        x = hsvResultMap_[SEARCHWINDOWID].x;
        y = hsvResultMap_[SEARCHWINDOWID].y;
        return true;
    }

    //find new point take care of other tracking results
    int sumMoment1_x = 0, sumMoment1_y = 0, sumMoment0 = 0;
    for (auto& d : hsvResultMap_)
    {
        if (d.first == windowId || d.first == SEARCHWINDOWID) continue;
        sumMoment0 += d.second.moment0;
        sumMoment1_x += d.second.moment1_x;
        sumMoment1_y += d.second.moment1_y;
    }

    int targetMoment0 = hsvResultMap_[SEARCHWINDOWID].moment0 - sumMoment0;
    int targetMoment1_x = hsvResultMap_[SEARCHWINDOWID].moment1_x - sumMoment1_x;
    int targetMoment1_y = hsvResultMap_[SEARCHWINDOWID].moment1_y - sumMoment1_y;
    if (targetMoment0 > 0 && targetMoment1_x > 0 && targetMoment1_y > 0)
    {
        //find new point centerX, centerY
        x = targetMoment1_x / static_cast<float>(targetMoment0);
        y = targetMoment1_y / static_cast<float>(targetMoment0);
        if (x < 0 || y < 0 || x > getWidth() || y > getHeight())
        {
            return false;
        }
    }
    else {
        return false;
    }

    //error check. new point is same with other tracking point
    if (findSamePositonResult(windowId, x, y) >= 0)
    {
        return false;
    }
    return true;
}

int SentechHsvSensorMultiTrack::findSamePositonResult(int windowId, float targetX, float targetY)
{
    double mostMinDist = kSamePointDistThre_;
    int ret = -1;
    float diffDist;
    for (const auto res : hsvResultMap_)
    {
        if (res.first == windowId || res.first == SEARCHWINDOWID) { continue; }
        //check detect position is same or not
        double diffX = targetX - res.second.x;
        double diffY = targetY - res.second.y;
        double diffDist = sqrt(diffX * diffX + diffY * diffY);
        if (diffDist < mostMinDist)
        {
            mostMinDist = diffDist;
            ret = res.first;
        }
    }
    return ret;
}
bool SentechHsvSensorMultiTrack::isSamePoint(float srcX, float srcY, float targetX, float targetY)
{
    double diffX = targetX - srcX;
    double diffY = targetY - srcY;
    double diffDist = sqrt(diffX * diffX + diffY * diffY);
    return diffDist < kSamePointDistThre_;
}

bool SentechHsvSensorMultiTrack::isPointInArea(float srcX, float srcY, float centX, float centY, float width, float height)
{
    return centX - width / 2 <= srcX && srcX <= centX + width / 2 &&
        centY - height / 2 <= srcY && srcY <= centY + height / 2;
}

bool SentechHsvSensorMultiTrack::exsistSamePosPoint(int windowId)
{
    int srcX = hsvResultMap_[windowId].x;
    int srcY = hsvResultMap_[windowId].y;
    for (const auto res : hsvResultMap_)
    {
        if (res.first == windowId || res.first == SEARCHWINDOWID) { continue; }
        //check detect position is same or not
        if (isSamePoint(srcX, srcY, res.second.x, res.second.y)){
            return true;
        }
    }
    return false;
}

bool SentechHsvSensorMultiTrack::isReadySearch(int windowId)
{
    for (auto& a : autoObjectDetectManager_)
    {
        if (a.id != windowId &&
            (a.objectSearchState == kAllAreaSearch ||
             a.objectSearchState == kRegistAreaSearch ||
             a.objectSearchState == kRegistAreaDetect ||
             a.objectSearchState == kLost))
        {
            return false;
        }
    }
    return true;
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
