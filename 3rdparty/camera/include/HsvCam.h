/*
* Copyright 2017 Sony Semiconductor Solutions Corporation.
*
* This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor Solutions Corporation.
* No part of this file may be copied, modified, sold, and distributed in any
* form or by any means without prior explicit permission in writing from
* Sony Semiconductor Solutions Corporation.
*
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "so/so_def.h"

#ifdef HSVCAM_EXPORTS
#define HSVCAM_API __declspec(dllexport)
#else
#define HSVCAM_API __declspec(dllimport)
#endif

namespace std
{
    class mutex;
}

namespace so {
	namespace imgproc
	{
		class viewer;
		class rec;
		class log;
	}
	namespace sensing
	{
		class Common;
		class Tracking;
	}
	namespace fpga
	{
		class Fpga;
	}
	namespace component
	{
		class IHsvComponent;
	}
	namespace util
	{
		class StopWatch;
        class SiLog;
    }

    class PacketParser;

    class HSVCAM_API HsvCam
    {

    public:
        enum InputMode_t
        {
            E_PICCOLO_USB_CAM,
            E_RAW_FILE
        };

        enum SensorDepth_t
        {
            E_RAW12,
            E_RAW10,
            E_RAW8,
            E_RAW8_4BIT,
            E_RAW8_1BIT
        };

        struct RecorderSaveTarget_t {
            bool raw;
            bool still;
            bool stillSi;
            bool movieActual;
            bool movieActualSi;
            bool movieSlow;
            bool movieSlowSi;
            bool siLog;
        };

        struct ImageViewerParam_t
        {
            bool gpuEn;
            bool ispEn;
            bool drawSi[TargetWindow::WinNum];
            bool drawGravPos;
            bool drawDetectArea;
            bool drawVector;
            bool drawMoment0;
            bool drawTrajectory;
            bool drawBinarizedImage;
            bool drawFrameRate;
            int logFrameNum;
        };

        HsvCam();
        HsvCam(int device_id, SystemSelect system, RegisterInterface reg_if);
        ~HsvCam();
        void setComPort(const std::string &comPortName);

        bool loadSetupFile(const std::string & fileName);
        bool setup(const std::string &paramSubDir);
        std::string &getParamRootPath(void);
        std::string &getParamPath(void);
        bool open(void);
        void close(void);
        bool startCapture(void);
        bool stopCapture(void);


        void setHFlip(bool en);
        bool isHFlip(void);
        void setVFlip(bool en);
        bool isVFlip(void);
        void setCropEn(bool en);
        bool getCropEn(void);
        void setCropOffset(uint32_t offsetX, uint32_t offsetY);
        void getCropOffset(uint32_t &offsetX, uint32_t &offsetY);
        void setCropOffsetX(uint32_t offsetX);
        uint32_t getCropOffsetX(void);
        void setCropOffsetY(uint32_t offsetY);
        uint32_t getCropOffsetY(void);
        void setCropSize(uint32_t width, uint32_t height);
        void getCropSize(uint32_t &width, uint32_t &height);
        void setCropWidth(uint32_t width);
        uint32_t getCropWidth(void);
        void setCropHeight(uint32_t width);
        uint32_t getCropHeight(void);
        bool getRawImage(cv::Mat &imgRaw, SensingInformationPacket_t &si);
        bool getRgbImage(cv::Mat &imgRgb, SensingInformationPacket_t &si);
        bool getRgbAndBinImage(cv::Mat &imgRgb, cv::Mat &imgBin, SensingInformationPacket_t &si);
        bool getSensingInformation(SensingInformationPacket_t &si);
        bool getRawImageNoFlip(cv::Mat &img, SensingInformationPacket_t &si);

        // sensor control
        void setupGainCtrl(const std::string &fileName);
        void setupColorDetect(const std::string &fileName);
        float getSensorFrameRate(void);
        void setAnalogGain(float analogGain);
        float getAnalogGain(void);

        void setFdGain(FdGain fdGain);
        FdGain getFdGain(void);

        void setShutterLine(uint16_t shutterLine);
        uint16_t getShutterLine(void);
        uint16_t getFrameLengthLines(void);
        void setWhiteBlanceGain(float gainR, float gainG, float gainB);
        void getWhiteBlanceGain(float &gainR, float &gainG, float &gainB);
        void setWhiteBlanceGainR(float gainR);
        float getWhiteBlanceGainR(void);
        void setWhiteBlanceGainG(float gainG);
        float getWhiteBlanceGainG(void);
        void setWhiteBlanceGainB(float gainB);
        float getWhiteBlanceGainB(void);

        uint16_t getSensorRomVersion(void);
        uint16_t getSensorParameterVersion(void);
        void getSensingFirmwareVersion(SensingFirmwareVersion_t &ver);

        void getPacketSize(int &width, int &height);
        void sensorChangeMode(SensorMode sensorMode);
        void sensorReboot(void);

        bool dumpSensorFlashRom(const std::string &file_name);
        bool writeSensorFlashRomSystem(const std::string &file_name);
        bool writeSensorFlashRomMode(const std::string &file_name);
        bool writeSensorFlashRom(const std::string &file_name, uint32_t start_address);
        bool writeSensorFlashValidCode(void);

        // image viewer
        void imageViewerSetup(const std::string &fileName);
        void imageViewerSetParam(const ImageViewerParam_t &par);
        void imageViewerGetParam(ImageViewerParam_t &par);
        void imageViewerSetDrawFrameRate(bool en);
        bool imageViewerGetDrawFrameRate(void);
        void imageViewerOpen(void);
        void imageViewerClose(void);
        void imageViewerShowImage(void);
        float imageViewerGetFrameRate(void);
        void imageViewerResetSi(void);
        void imageViewerSetCropEn(bool en);
        bool imageViewerGetCropEn(void);
        void imageViewerSetCrop(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        void imageViewerSetCropOffsetX(uint32_t x);
        void imageViewerSetCropOffsetY(uint32_t y);
        void imageViewerSetCropWidth(uint32_t width);
        void imageViewerSetCropHeight(uint32_t height);
        void imageViewerGetCrop(uint32_t &x, uint32_t &y, uint32_t &width, uint32_t &height);
        uint32_t imageViewerGetCropOffsetX(void);
        uint32_t imageViewerGetCropOffsetY(void);
        uint32_t imageViewerGetCropWidth(void);
        uint32_t imageViewerGetCropHeight(void);

        // image recorder
        enum RecordingStatus_t
        {
            E_STOP,
            E_WAIT_TRIGGER,
            E_RECODING,
            E_RECODING_POST,
            E_SAVING,
            E_PLAYING,
            E_PAUSE
        };
        void imageRecorderSetup(const std::string &fileName);
        RecordingStatus_t imageRecorderGetStatus(void);
        void imageRecorderGetRootDirPath(std::string &path);
        void imageRecorderSetDirPath(const std::string &path);
        void imageRecorderGetSaveTarget(RecorderSaveTarget_t &saveTarget);
        void imageRecorderSetSaveTarget(const RecorderSaveTarget_t &saveTarget);
        void imageRecorderStartRecording(int32_t frameNum, bool autoSave);
        void imageRecorderStartWaitTrigger(int32_t frameNumPre, int32_t frameNumMin, int32_t frameNumMax, int32_t frameNumPost, bool autoSave);
        void imageRecorderPlay(void);
        void imageRecorderStop(void);
        void imageRecorderPause(void);
        void imageRecorderDec(void);
        void imageRecorderInc(void);
        void imageRecorderStartSaveFiles(void);

        // si logger
        void siLoggerSetup(const std::string &fileName);
        void siLoggerGetRootDirPath(std::string &path);
        void siLoggerSetDirPath(const std::string &path);
        void siLoggerStartRecording(int32_t frameNum);

        // adjustment
        void onePushWhiteBalance(void);

		// sensor
		bool sensorOpen(SensorMode sensorMode);

        // tracking
        void trackingInit(void);
        void trackingRun(void);
        void trackingSetWinDispEn(TargetWindow windowSelect, bool en);
        bool trackingGetWinDispEn(TargetWindow windowSelect);
        void trackingSetWinArea(TargetWindow windowSelect, WindowArea mode);
        WindowArea trackingGetWinArea(TargetWindow windowSelect);
        void trackingSetWinDispMode(TargetWindow windowSelect, WindowDisplayMode mode);
        WindowDisplayMode trackingGetWinDispMode(TargetWindow windowSelect);
        void trackingSetWinPosMode(TargetWindow windowSelect, WindowPositionMode mode);
        WindowPositionMode trackingGetWinPosMode(TargetWindow windowSelect);
        void trackingSetWindowSize(TargetWindow windowSelect, uint16_t width, uint16_t height);
        void trackingGetWindowSize(TargetWindow windowSelect, uint16_t &width, uint16_t &height);
        void trackingSetWindowWidth(TargetWindow windowSelect, uint16_t width);
        uint16_t trackingGetWindowWidth(TargetWindow windowSelect);
        void trackingSetWindowHeight(TargetWindow windowSelect, uint16_t height);
        uint16_t trackingGetWindowHeight(TargetWindow windowSelect);
        void trackingSetWindowPosX(TargetWindow windowSelect, uint16_t x);
        void trackingSetWindowPosY(TargetWindow windowSelect, uint16_t y);
        void trackingSetWindowPos(TargetWindow windowSelect, uint16_t x, uint16_t y);
        void trackingGetWindowPos(TargetWindow windowSelect, uint16_t &x, uint16_t &y);
        void trackingSetWindowColorEn(TargetWindow windowSelect, bool r_en, bool g_en, bool b_en);
        void trackingSetWindowColorEnR(TargetWindow windowSelect, bool r_en);
        void trackingSetWindowColorEnG(TargetWindow windowSelect, bool g_en);
        void trackingSetWindowColorEnB(TargetWindow windowSelect, bool b_en);
        void trackingGetWindowColorEn(TargetWindow windowSelect, bool &r_en, bool &g_en, bool &b_en);
        void trackingSetWindowMaskImage(TargetWindow windowSelect, int mask_img);
        int trackingGetWindowMaskImage(TargetWindow windowSelect);
        void trackingSetWinAllMask0(void);
        void trackingSetWinAllMask1(void);
        void trackingSetMcPixSize(int width, int height);
        void trackingSetObjSizeMin(TargetWindow windowSelect, uint32_t min);
        uint32_t trackingGetObjSizeMin(TargetWindow windowSelect);
        void trackingSetMvMin(TargetWindow windowSelect, uint32_t min);
        uint32_t trackingGetMvMin(TargetWindow windowSelect);
        void trackingSetMvMax(TargetWindow windowSelect, uint32_t max);
        uint32_t trackingGetMvMax(TargetWindow windowSelect);
        void trackingSetWindowDispCondition(TargetWindow windowSelect, const WindowDisplayParam &par);
        void trackingGetWindowDispCondition(TargetWindow windowSelect, WindowDisplayParam &par);
        void trackingSetWindowDispConditionAnd(TargetWindow windowSelect, bool useSize, bool useMvMgn, bool useMvDir);
        void trackingGetWindowDispConditionAnd(TargetWindow windowSelect, bool &useSize, bool &useMvMgn, bool &useMvDir);
        void trackingSetWindowDispConditionOr(TargetWindow windowSelect, bool useSize, bool useMvMgn, bool useMvDir);
        void trackingGetWindowDispConditionOr(TargetWindow windowSelect, bool &useSize, bool &useMvMgn, bool &useMvDir);
        void trackingSetColorDetectEn(bool en);
        bool trackingGetColorDetectEn(void);
        void trackingSetColorDetectMode(ColorDetectMode mode);
        ColorDetectMode trackingGetColorDetectMode(void);
        void trackingSetColorDetectRatio(const ColorDetectRatio &par);
        void trackingGetColorDetectRatio(ColorDetectRatio &par);
        void trackingSetColorDetectBase(ColorDetectBase base);
        void trackingSetColorDetectAlpha(float alpha);
        void trackingSetColorDetectBeta(float beta);
        void trackingSetColorDetectGammaMin(uint8_t gammaMin);
        void trackingSetColorDetectGammaMax(uint8_t gammaMax);
        void trackingSetColorDetectRgbMaxMin(const ColorDetectMaxMin &par);
        void trackingGetColorDetectRgbMaxMin(ColorDetectMaxMin &par);
        void trackingSetMonoTrackingMin(uint8_t mono_min);
        void trackingSetMonoTrackingMax(uint8_t mono_max);
        void trackingSetMonoTrackingParam(uint8_t min, uint8_t max);
        void trackingGetMonoTrackingParam(uint8_t &min, uint8_t &max);
        void trackingGetResult(int &result, int &moment0, int &grav_x, int &grav_y);
        void trackingSetFwHold(bool en);
        bool trackingGetFwHold(void);
        void trackingSetWindowCtrl(TargetWindow windowSelect, bool en);
        bool trackingGetWindowCtrl(TargetWindow windowSelect);
        void trackingSetWindowOverlapCtrl(TargetWindow windowSelect, bool en);
        bool trackingGetWindowOverlapCtrl(TargetWindow windowSelect);
        void trackingSetWindowRatio(float ratio);
        float trackingGetWindowRatio(void);
        void trackingSetWindowMin(uint16_t min);
        uint16_t trackingGetWindowMin(void);
        void trackingSetWindowFrame(uint32_t line_width, uint32_t r, uint32_t g, uint32_t b);
        void trackingSetTargetMode(TargetMode target_mode);
        void trackingSetSearchPos(TargetWindow windowSelect, uint16_t x, uint16_t y);
        void trackingGetSearchPos(TargetWindow windowSelect, uint16_t &x, uint16_t &y);
        void trackingSetSearchPosX(TargetWindow windowSelect, uint16_t x);
        uint16_t trackingGetSearchPosX(TargetWindow windowSelect);
        void trackingSetSearchPosY(TargetWindow windowSelect, uint16_t y);
        uint16_t trackingGetSearchPosY(TargetWindow windowSelect);
        void trackingSetSearchSize(TargetWindow windowSelect, uint16_t width, uint16_t height);
        void trackingGetSearchSize(TargetWindow windowSelect, uint16_t &width, uint16_t &height);
        void trackingSetSearchWidth(TargetWindow windowSelect, uint16_t width);
        uint16_t trackingGetSearchWidth(TargetWindow windowSelect);
        void trackingSetSearchHeight(TargetWindow windowSelect, uint16_t height);
        uint16_t trackingGetSearchHeight(TargetWindow windowSelect);
        void trackingSetWHRatio(TargetWindow windowSelect, float ratio);
        float trackingGetWHRatio(TargetWindow windowSelect);
        void trackingSetSearchPosRatio(TargetWindow windowSelect, float centerXRatio, float centerYRatio);
        void trackingGetSearchPosRatio(TargetWindow windowSelect, float &centerXRatio, float &centerYRatio);
        void trackingSetSearchPosRatioX(TargetWindow windowSelect, float centerXRatio);
        float trackingGetSearchPosRatioX(TargetWindow windowSelect);
        void trackingSetSearchPosRatioY(TargetWindow windowSelect, float centerYRatio);
        float trackingGetSearchPosRatioY(TargetWindow windowSelect);
        void trackingSetSearchSizeRatio(TargetWindow windowSelect, float widthRatio, float heightRatio);
        void trackingGetSearchSizeRatio(TargetWindow windowSelect, float &widthRatio, float &heightRatio);
        void trackingSetSearchWidthRatio(TargetWindow windowSelect, float widthRatio);
        float trackingGetSearchWidthRatio(TargetWindow windowSelect);
        void trackingSetSearchHeightRatio(TargetWindow windowSelect, float heightRatio);
        float trackingGetSearchHeightRatio(TargetWindow windowSelect);
        void trackingSetOutputImageSelect(OutputImageSelect outputImageSelect);
        OutputImageSelect trackingGetOutputImageSelect(void);

        void siOutSetup(const std::string &fileName);
        void siOutSetMode(SiOutMode mode);
        SiOutMode siOutGetMode(void);
        void siOutSetCsMode(SiOutCsMode endian);
        SiOutCsMode siOutGetCsMode(void);
        void siOutSetHFlip(bool en);
        bool siOutGetHFlip(void);
        void siOutSetVFlip(bool en);
        bool siOutGetVFlip(void);
        void siOutSetScalingFactor(float scalingFactor);
        float siOutGetScalingFactor(void);
        void siOutSetNum(uint8_t num);
        uint8_t siOutGetNum(void);
        void siOutSetFramePeriod(uint8_t framePeriod);
        uint8_t siOutGetFramePeriod(void);
        void siOutSetImageWidth(uint16_t width);
        uint16_t siOutGetImageWidth(void);
        void siOutSetImageHeight(uint16_t height);
        uint16_t siOutGetImageHeight(void);

        // other
        void setGpuEn(bool en);
        bool getGpuEn(void);
        int64_t getTimeRawStreamMicroSec(void);
        float getTimeRawStreamFrameRate(void);
        void setSavePacketEn(bool en);
        uint64_t getTotalFrameCount(void);
        uint64_t getLostFrameCount(void);

        void convertImage(cv::Mat &imgRaw, SensingInformationPacket_t &si, cv::Mat &imgOut);

        uint8_t readSensRegU8(uint32_t addr);
        uint16_t readSensRegU16(uint32_t addr);
        uint32_t readSensRegU32(uint32_t addr);
        uint32_t readSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb);
        void writeSensRegU8(uint32_t addr, uint8_t val);
        void writeSensRegU16(uint32_t addr, uint16_t val);
        void writeSensRegU32(uint32_t addr, uint32_t val);
        void writeSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val);
        uint8_t readFpgaRegU8(uint32_t addr);
        uint16_t readFpgaRegU16(uint32_t addr);
        uint32_t readFpgaRegU32(uint32_t addr);
        uint32_t readFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb) ;
        void writeFpgaRegU8(uint32_t addr, uint8_t val);
        void writeFpgaRegU16(uint32_t addr, uint16_t val);
        void writeFpgaRegU32(uint32_t addr, uint32_t val);
        void writeFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val);

        struct DriverInfo {
            std::string dll_ver;
            std::string sys_ver;
            std::string firmware_ver;
        };
        void getDriverInfo(DriverInfo &info);

    private:
        void initComponent(int device_id, InputMode_t inputMode, SystemSelect system, RegisterInterface reg_if);


        bool init(SensorMode sensorMode);


        void updateFrame(const uint8_t *pPacket, int32_t packetSize);
        bool makeFpgaConfig(SensorMode sensorMode, Config_t &config);
        bool sensorSetup(const std::string & fileName);
        void enqueue(const cv::Mat &img);
        void enqueue(const SensingInformationPacket_t &si);
        void clearQue(void);

        // common
        void loadSensorSetting(void);
        std::string *m_paramRootPath;
        std::string *m_paramPath;
        InputMode_t m_inputMode;
        bool m_isCameraOpened;
        bool m_isViewerOpened;
        float m_SensorFrameRate;
        bool m_hFlip;
        bool m_vFlip;
        bool m_cropEn;
        uint32_t m_cropOffsetX;
        uint32_t m_cropOffsetY;
        uint32_t m_cropWidth;
        uint32_t m_cropHeight;

        // sensing
        float m_windowRatio;
        float m_whRatio[WinNum];

        // image viewer
        so::imgproc::viewer *m_viewer;
        void imageViewerInit(void);
        void imageViewerSetupColorDetect(void);
        bool imageViewerGetImage(cv::Mat &img, SensingInformationPacket_t &si);
        void imageViewerRecordingStatus(cv::InputOutputArray img);
        void imageViewerDrawFrameRate(cv::InputOutputArray img);

        // image recorder
        so::imgproc::rec *m_recorder;
        void imageRecorderInit(void);
        void imageRecoderParseRequest(void);
        void imageRecorder(void);
        bool imageRecorderGetData(cv::Mat &img, SensingInformationPacket_t &si);

        RecordingStatus_t imageRecorderWaitTrigger(RecordingStatus_t sts_now);
        RecordingStatus_t imageRecorderRecording(RecordingStatus_t sts_now);
        RecordingStatus_t imageRecorderRecordingPost(RecordingStatus_t sts_now);
        RecordingStatus_t imageRecorderSaving(RecordingStatus_t sts_now, RecordingStatus_t sts_1z);

        bool imageRecorderJudgeEnd(bool si_result);
        void imageRecorderMakeDirectory(void);
        void imageRecorderSaveFiles(void);

        // raw data stream
        bool isRecieved(uint32_t frameCount, uint32_t frameCountBuf);

        so::util::StopWatch *m_stopWatchRawStream;

        enum
        {
            TIME_RAW_STRM_NUM = 128
        };
        int64_t m_TimeRawStreamMicroSec[TIME_RAW_STRM_NUM];
        int m_TimeRawStreamIndex;

        PacketParser *m_packetParser;
        std::vector<cv::Mat> *m_imgQue;
        std::vector<SensingInformationPacket_t> *m_siQue;
        int32_t m_imgQueMax;
        int32_t m_siQueMax;
        uint32_t m_frameCountBuf;

        // viewer
        bool m_drawFrameRate;
        cv::Mat *m_imgOut;
        so::util::StopWatch *m_imageViewerStopWatch;
        int64_t m_imageViewerTimeMicroSec;
        uint32_t m_viewerFrameCntBuf;

        // image recorder
        std::string *m_imageRecorderRootDirPath;
        RecorderSaveTarget_t m_imageRecorderSaveTarget;
        so::util::StopWatch *m_imageRecorderStopWatch;
        int64_t m_imageRecorderTimeMilliSec;
        uint32_t m_recorderFrameCntBuf;


        enum RecordingMode_t
        {
            E_NORMAL,
            E_TRIG
        };

        enum
        {
            SI_REC_RESULT_BUF_NUM = 3
        };

        enum ImageRecorderRequest_t {
            E_REQ_NONE,
            E_REQ_PLAY,
            E_REQ_PAUSE,
            E_REQ_DEC,
            E_REQ_INC
        };
        ImageRecorderRequest_t m_ImageRecorderRequest;

        struct RecordingViewerParam_t
        {
            RecordingMode_t mode;
            RecordingStatus_t sts;
            RecordingStatus_t sts_prev;
            std::string path;
            bool trig_start;
            int frm_pre;
            int frm_min;
            int frm_max;
            int frm_post;
            int cnt;
            int cnt_post;
            bool result_buf[SI_REC_RESULT_BUF_NUM];
            bool trig_end_en;
            int judge_end_cnt;
            bool auto_save;
            int  play_cnt;
            int64_t ticks;
        };
        RecordingViewerParam_t m_rec;


        // si logger
        void siLoggerInit(void);
        void siLogger(void);

        so::util::SiLog *m_logger;

        struct SiLogParam
        {
            bool en;
            int frm_num;
            int cnt;
            std::string path;
        };
        SiLogParam m_log;


		// utility
		void sleepMilliSec(uint32_t time);

        int32_t m_packet_width;
        int32_t m_packet_height;

        bool m_setupViewer;
        bool m_setupRecorder;
        bool m_setupGainCtrl;
        bool m_setupColorDetect;
        bool m_setupSpi;

        SensingFirmwareVersion_t m_SensingFirmwareVersion;

        enum
        {
            FORMAT_TABLE_NUM = 6,
        };

        static const Config_t m_fpgaConfigTable[FORMAT_TABLE_NUM];

        void savePacket(const std::string &fileName, const uint8_t *pPacket);
        bool m_savePacket;


        uint64_t m_totalFrameCount;
        uint64_t m_lostFrameCount;
        int32_t m_frameCountDiff;

        RegisterInterface m_registerInterfaceSel;
        std::string *m_comPortName;

        std::string *m_viewerWindowName;
        bool m_gpu_en;

		SystemSelect m_systemSel;

		so::component::IHsvComponent* m_hsv_component;
		so::sensing::Common* m_sensing_common;
		so::sensing::Tracking* m_sensing_tracking;
		so::fpga::Fpga* m_fpga;

        std::mutex *m_mtxImg;
        std::mutex *m_mtxSi;

        int32_t calcFrameCountDiff(uint32_t frameCountCur, uint32_t frameCountPrev);

        template<class T>
        void deleteSafe(T*& p)
        {
            delete p;
            p = nullptr;
        }
    };
}
