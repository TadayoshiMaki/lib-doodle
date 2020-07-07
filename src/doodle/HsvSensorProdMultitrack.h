#ifndef LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProdMultitrack_H_
#define LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProdMultitrack_H_

#include "doodle/namespace.h"
#include "doodle/HsvSensorProd.h"
#include <map>
//#define DEBUG_CAMIMAGE_

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using StApi::CIStDataStreamPtr;
using StApi::CIStDevicePtr;
using StApi::CIStStreamBufferPtr;
using StApi::CIStSystemPtr;
using StApi::CStApiAutoInit;
using StApi::IStImage;
using StApi::IStPixelFormatInfo;
using StApi::EStPixelFormatNamingConvention_t;
using GenApi::CNodeMapPtr;

class SentechHsvSensorMultiTrack : public SentechHsvSensor
{
public:
    SentechHsvSensorMultiTrack();
    virtual ~SentechHsvSensorMultiTrack();

    /**
        * @brief Sets whether frame capturing is enabled.
        * @param enable true to set this sensor to use frame capturing, false otherwise.
        */
    void setCaptureEnabled(bool enabled);

    void setCanFindNewPoint(bool value);
protected:
    void run() override;

private:
    /**
    * @enum hsv camera object detect parameter (ObjectDetectionFrameSize) 
    * @brief parameter of object detect area frame size mode
    */
    typedef enum {
        kObjectDetectionFrameSizeAll = 0,
        kObjectDetectionFrameSizeRegister,
        kObjectDetectionFrameSizeOff,
        kObjectDetectionFrameSizeNone
    } ObjectDetectionFrameSize;

    /**
    * @enum hsv camera object detect parameter (ObjectDetectionFrameCenterPositonMode)
    * @brief parameter of object detect center position mode
    */
    typedef enum {
        kObjectDetectionFrameCenterPositonModeAlways = 0,
        kObjectDetectionFrameCenterPositonModeDetected,
        kObjectDetectionFrameCenterPositonModeNone
    } ObjectDetectionFrameCenterPositonMode;

    /**
    * @enum hsv camera object detect parameter (ObjectDetectionFrameCenterPositonMode)
    * @brief parameter of display mode of object detect result
    */
    typedef enum {
        kObjectDetectionFrameDisplayModeAlways = 0,
        kObjectDetectionFrameDisplayModeDetected,
        kObjectDetectionFrameDisplayModeNone
    } ObjectDetectionFrameDisplayMode;

    /**
    * @enum hsv camera object detect parameter (ObjectTrackingFrameMode)
    * @brief parameter of object tracking mode (Manual is Only detect, Auto is detect and after track)
    */
    typedef enum {
        kObjectTrackingFrameModeManual = 0,
        kObjectTrackingFrameModeAuto,
        kObjectTrackingFrameModeNone
    } ObjectTrackingFrameMode;

    //-----------------------------------------------------------------------------
    typedef enum {
        kObjectDetectionFrameCenterPositionModeRegister = 0,
        kObjectDetectionFrameCenterPositionModeGravityPosition,
        kObjectDetectionFrameCenterPositionModeAverageGravityPosition,
        kObjectDetectionFrameCenterPositionModeNone
    } ObjectDetectionFrameCenterPositionMode;

    /**
    * @enum hsv camera multi object detect state (lib-doodle-fw internal use)
    */
    typedef enum {
        kWaiting = 0,	    // Waiting
        kAllAreaSearch, // All area search
        kRegistAreaSearch,	// Regist area search
        kRegistAreaDetect,	// Regist area detect
        kRegistAreaTrack,   // Regist area track
        kLost,              // Lost
        kUnknown             //Unknown
    } ObjectSearchState;

    /**
    * @struct hsv camera multi object detect struct data(lib-doodle-fw internal use)
    */
    struct hsvResult
    {
        bool isDetect;
        uint8_t trackingId;
        float x;
        float y;
        int64_t timeMicros;
        int moment0;
        int moment1_x;
        int moment1_y;
        hsvResult() {}
        hsvResult(bool isDetect, uint8_t trackingId, float x, float y, int64_t timeMicros, int moment0, int moment1_x, int moment1_y) :
            isDetect(isDetect), trackingId(trackingId), x(x), y(y), timeMicros(timeMicros),
            moment0(moment0), moment1_x(moment1_x), moment1_y(moment1_y)
        {
        }
    };

    /**
    * @struct hsv camera multi object detect manager (lib-doodle-fw internal use)
    */
    struct AutoObjectDetectManager
    {
        int id;
        ObjectSearchState objectSearchState;
        float searchX, searchY, searchW, searchH;
        int stableCounter;
        AutoObjectDetectManager(int id, ObjectSearchState state) : id(id), objectSearchState(state), searchX(0.0f), searchY(0.0f), searchW(0.0f), searchH(0.0f), stableCounter(0)
        {
        }
        AutoObjectDetectManager(int id) : id(id), objectSearchState(kLost), searchX(0.0f), searchY(0.0f), searchW(0.0f), searchH(0.0f), stableCounter(0)
        {
        }
    private:
        //AutoObjectDetectManager(const AutoObjectDetectManager&) = delete;
        //AutoObjectDetectManager(AutoObjectDetectManager&&) = delete;
        AutoObjectDetectManager& operator=(const AutoObjectDetectManager&) = delete;
        //AutoObjectDetectManager& operator=(AutoObjectDetectManager&&) = delete;
    };

    /**
    * @brief finds the new object
    * @param windowId is hsv sensor object detect regionId
    * @param x is new point X coordinate(if new object is find)
    * @param y is new point Y coordinate(if new object is find)
    * @return true if successfully find new object.
    */
    bool findNewPoint(int windowId, float& x, float& y);

    /**
    * @brief check the same point(targetX, targetY) exists in other regions.
    * @param windowId is hsv sensor object detect regionId
    * @param targetX is target point X coordinate
    * @param targetY is target point Y coordinate
    * @return if find same point, then retuen regionId, else return -1(Inval Id)
    */
    int findSamePositonResult(int windowId, float targetX, float targetY);

    /**
    * @brief check the same point or Not.
    * @param srcX is src point X coordinate
    * @param srcY is src point Y coordinate
    * @param targetX is target point X coordinate
    * @param targetY is target point Y coordinate
    * @return true if src and target point is same coordinate
    */
    bool isSamePoint(float srcX, float srcY, float targetX, float targetY);

    bool isPointInArea(float srcX, float srcY, float centX, float centY, float width, float height);

    bool exsistSamePosPoint(int windowId);

    /**
    * @brief check it is possible to start search new object.
    * @param windowId is hsv sensor object detect regionId
    * @return true if windowId region can start search new object
    */
    bool isReadySearch(int windowId);

    // The list of hsv result map (only internal use)
    std::map<int, hsvResult> hsvResultMap_;
    // The list of multi object detect manager (only internal use)
    std::vector<AutoObjectDetectManager> autoObjectDetectManager_;

    // The threshold of same point distance (fixed value)
    static const int kSamePointDistThre_ = 50;
    //The search area size. (x, y) = (searchAreaSize_, searchAreaSize_)
    static const int kSearchAreaSize_ = 150;
    //The tracking area size
    static const int kTrackingAreaSize_ = 50;
    static const int kRregisterDetectionWaitFrameCountThre_ = 150;
    int SEARCHWINDOWID = -1;
    bool canFindingNewPoint_ = true;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_HsvSensorProdMultitrack_H_
