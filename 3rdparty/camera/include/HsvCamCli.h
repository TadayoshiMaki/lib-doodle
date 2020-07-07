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

namespace so {

    class HsvCam;

	public ref class HsvCamCli
	{
    public:
        enum class InputMode_t : int
        {
            E_PICCOLO_USB_CAM,
            E_RAW_FILE
        };

        ref struct SensingFirmwareVersion_t
        {
            int major;
            int minor;
            int buildNumber;
            int revision;
        };

        enum class SensorMode : int
        {
            FULL_10BIT_0120FPS = 3,
            FULL_08BIT_0120FPS = 48,
            VH12_08BIT_0240FPS = 16,
            FULL_04BIT_0500FPS = 7,
            VH12_04BIT_1000FPS = 18,
        };

        enum class SensorDepth_t : int
        {
            E_RAW12,
            E_RAW10,
            E_RAW8,
            E_RAW8_4BIT,
            E_RAW8_1BIT
        };

        ref struct RecorderSaveTarget_t
        {
            System::Boolean raw;
            System::Boolean still;
            System::Boolean stillSi;
            System::Boolean movieActual;
            System::Boolean movieActualSi;
            System::Boolean movieSlow;
            System::Boolean movieSlowSi;
            System::Boolean siLog;
        };

        enum class TargetWindow : int
        {
            Win1,
            Win2,
            Win3,
            Win4,
            Win5,
            Win6,
            WinNum,
            WinMin = Win1,
            WinMax = Win6,
        };

        ref struct ImageViewerParam_t
        {
            System::Boolean gpuEn;
            System::Boolean ispEn;
            cli::array<System::Boolean> ^drawSi;
            System::Boolean drawGravPos;
            System::Boolean drawDetectArea;
            System::Boolean drawVector;
            System::Boolean drawMoment0;
            System::Boolean drawTrajectory;
            System::Boolean drawBinarizedImage;
            System::Boolean drawFrameRate;
            System::UInt32 logFrameNum;
        };

        ref struct Position_t
        {
            System::UInt16 x;
            System::UInt16 y;
        };

        ref struct SensingInformation_t
        {
            System::Boolean result;
            System::UInt32 moment0;
            System::UInt32 moment1_x;
            System::UInt32 moment1_y;
            Position_t grvpos;
            Position_t ave_grvpos;
            Position_t mvpos;
            System::UInt16 mvmgn;
            System::Byte  mvdir;
        };

        ref struct SensingInformationPacket_t
        {
            System::UInt32 frameCount;
            cli::array<SensingInformation_t^> ^win;
        };

        enum class RecordingStatus_t : int
        {
            E_STOP,
            E_WAIT_TRIGGER,
            E_RECODING,
            E_RECODING_POST,
            E_SAVING,
            E_PLAYING,
            E_PAUSE
        };

		enum class SystemSelect : int
		{
			PiccoloX,
			SDK
		};

        enum class RegisterInterface : int
        {
            USB3,
            UART,
			USB_UVC,
        };

		HsvCamCli();
		HsvCamCli(int device_id, SystemSelect system, RegisterInterface reg_if);
		~HsvCamCli();

        System::IntPtr GetPtr(System::Void);

        System::Void setComPort(System::String ^comPortName);

		System::Boolean loadSetupFile(System::String ^fileName);
		System::Boolean setup(System::String ^paramSubDir);
		System::String^ getParamRootPath(System::Void);
        System::String^ getParamPath(System::Void);
        System::Boolean open(System::Void);
		System::Void close(System::Void);
		System::Boolean startCapture(System::Void);
		System::Boolean stopCapture(System::Void);
        System::Void setHFlip(System::Boolean en);
        System::Boolean isHFlip(System::Void);
        System::Void setVFlip(System::Boolean en);
        System::Boolean isVFlip(System::Void);

        System::Void setCropEn(System::Boolean en);
        System::Boolean getCropEn(System::Void);
        System::Void setCropOffset(System::UInt32 offsetX, System::UInt32 offsetY);
        System::Void getCropOffset(System::UInt32 &offsetX, System::UInt32 &offsetY);
        System::UInt32 getCropOffsetX(System::Void);
        System::UInt32 getCropOffsetY(System::Void);
        System::Void setCropSize(System::UInt32 width, System::UInt32 height);
        System::Void getCropSize(System::UInt32 &width, System::UInt32 &height);
        System::UInt32 getCropWidth(System::Void);
        System::UInt32 getCropHeight(System::Void);

		System::Boolean getRawImage(cv::Mat %img, SensingInformationPacket_t %si);
		System::Boolean getSensingInformation(SensingInformationPacket_t %si);

		// sensor control
		System::Void setupGainCtrl(System::String ^fileName);
		System::Void setupColorDetect(System::String ^fileName);
		System::Single getSensorFrameRate(System::Void);
		System::Void setAnalogGain(System::Single analogGain);
		System::Single getAnalogGain(System::Void);
        enum class FdGain : int
        {
            Low,
            High
        };
        System::Void setFdGain(FdGain fdGain);
        FdGain getFdGain(System::Void);
        System::Void setShutterLine(System::UInt16 shutterLine);
		System::UInt16 getShutterLine(System::Void);
        System::UInt16 getFrameLengthLines(System::Void);
        System::Void setWhiteBlanceGain(System::Single gainR, System::Single gainG, System::Single gainB);
		System::Void getWhiteBlanceGain(System::Single %gainR, System::Single %gainG, System::Single %gainB);
        System::Void setWhiteBlanceGainR(System::Single gainR);
        System::Single getWhiteBlanceGainR(System::Void);
        System::Void setWhiteBlanceGainG(System::Single gainG);
        System::Single getWhiteBlanceGainG(System::Void);
        System::Void setWhiteBlanceGainB(System::Single gainB);
        System::Single getWhiteBlanceGainB(System::Void);
                
        System::UInt16 getSensorRomVersion(System::Void);
        System::UInt16 getSensorParameterVersion(System::Void);
        System::Void getSensingFirmwareVersion(SensingFirmwareVersion_t %ver);

        System::Void getPacketSize(System::Int32 %width, System::Int32 %height);
		System::Void sensorChangeMode(SensorMode sensorMode);

        System::Boolean dumpSensorFlashRom(System::String ^file_name);
        System::Boolean writeSensorFlashRomSystem(System::String ^file_name);
        System::Boolean writeSensorFlashRomMode(System::String ^file_name);
        System::Boolean writeSensorFlashRom(System::String ^file_name, System::UInt32 start_address);
        System::Boolean writeSensorFlashValidCode(System::Void);

		// image viewer
		System::Void imageViewerSetup(System::String ^fileName);
		System::Void imageViewerSetParam(ImageViewerParam_t %par);
		System::Void imageViewerGetParam(ImageViewerParam_t %par);
		System::Void imageViewerSetDrawFrameRate(System::Boolean en);
		System::Boolean imageViewerGetDrawFrameRate(System::Void);
		System::Void imageViewerOpen(System::Void);
		System::Void imageViewerClose(System::Void);
		System::Void imageViewerShowImage(System::Void);
		System::Single imageViewerGetFrameRate(System::Void);
		System::Void imageViewerResetSi(System::Void);
        System::Void imageViewerSetCropEn(System::Boolean en);
        System::Boolean imageViewerGetCropEn(System::Void);
        System::Void imageViewerSetCrop(System::UInt32 x, System::UInt32 y, System::UInt32 width, System::UInt32 height);
        System::Void imageViewerSetCropOffsetX(System::UInt32 x);
        System::Void imageViewerSetCropOffsetY(System::UInt32 y);
        System::Void imageViewerSetCropWidth(System::UInt32 x);
        System::Void imageViewerSetCropHeight(System::UInt32 y);
        System::Void imageViewerGetCrop(System::UInt32 %x, System::UInt32 %y, System::UInt32 %width, System::UInt32 %height);
        System::UInt32 imageViewerGetCropOffsetX(System::Void);
        System::UInt32 imageViewerGetCropOffsetY(System::Void);
        System::UInt32 imageViewerGetCropWidth(System::Void);
        System::UInt32 imageViewerGetCropHeight(System::Void);

		// image recorder
		System::Void imageRecorderSetup(System::String ^fileName);
		RecordingStatus_t imageRecorderGetStatus(System::Void);
        System::String ^imageRecorderGetRootDirPath(System::Void);
		System::Void imageRecorderSetDirPath(System::String ^path);
        System::Void imageRecorderGetSaveTarget(RecorderSaveTarget_t %saveTarget);
        System::Void imageRecorderSetSaveTarget(RecorderSaveTarget_t %saveTarget);
		System::Void imageRecorderStartRecording(System::Int32 frameNum, System::Boolean autoSave);
		System::Void imageRecorderStartWaitTrigger(System::Int32 frameNumPre, System::Int32 frameNumMin, System::Int32 frameNumMax, System::Int32 frameNumPost, System::Boolean autoSave);
		System::Void imageRecorderPlay(System::Void);
		System::Void imageRecorderStop(System::Void);
		System::Void imageRecorderPause(System::Void);
		System::Void imageRecorderDec(System::Void);
		System::Void imageRecorderInc(System::Void);
		System::Void imageRecorderStartSaveFiles(System::Void);

        // si logger
        System::Void siLoggerSetup(System::String ^fileName);
        System::String^ siLoggerGetRootDirPath(System::Void);
        System::Void siLoggerSetDirPath(System::String ^path);
        System::Void siLoggerStartRecording(System::Int32 frameNum);

		// adjustment
		System::Void onePushWhiteBalance(System::Void);

		// sensor
		System::Boolean sensorOpen(SensorMode sensorMode);

        // tracking
        enum class WindowArea : int
        {
            All,
            Local
        };

        enum class WindowDisplayMode : int
        {
            Always,
            Detected
        };

        enum class WindowPositionMode : int
        {
            Reg,
            GrvPos,
            AveGrvPos,
        };

        enum class WindowDisplayCondition : int
        {
            Mode3And,
            Mode3Or,
            Mode2And,
            Mode2Or
        };

        ref struct WindowDisplayParam
        {
            WindowDisplayCondition mode;
            bool useSize;
            bool useMvMgn;
            bool useMvDir;
        };

        enum class ColorDetectMode : int
        {
            MinMax,
            ColorRatio
        };

        enum class ColorDetectBase : int
        {
            Red,
            Green,
            Blue,
        };

        ref struct ColorDetectRatio
        {
            ColorDetectBase base;
            System::Single alpha;
            System::Single beta;
            System::Byte gammaMin;
            System::Byte gammaMax;
        };
        
        ref struct ColorDetectMaxMin
        {
            System::UInt32 rMax;
            System::UInt32 rMin;
            System::UInt32 gMax;
            System::UInt32 gMin;
            System::UInt32 bMax;
            System::UInt32 bMin;
        };

        enum class TargetMode : int
        {
            MemoryPreset,
            BackgroundDiff,
            FrameDiff,
            FrameDiffKeep,
            Binary,
            Reserved5,
            FrameDiffBinary,
            SelfWindow
        };

        enum class OutputImageSelect : int
        {
            Viewing,
            Sensing4bit,
            SensingBinarized1,
            SensingBinarized2
        };

        System::Void trackingInit(System::Void);
        System::Void trackingRun(System::Void);
        System::Void trackingSetWinDispEn(TargetWindow windowSelect, System::Boolean en);
        System::Boolean trackingGetWinDispEn(TargetWindow windowSelect);
        System::Void trackingSetWinArea(TargetWindow windowSelect, WindowArea mode);
        WindowArea trackingGetWinArea(TargetWindow windowSelect);
        System::Void trackingSetWinDispMode(TargetWindow windowSelect, WindowDisplayMode mode);
        WindowDisplayMode trackingGetWinDispMode(TargetWindow windowSelect);
        System::Void trackingSetWinPosMode(TargetWindow windowSelect, WindowPositionMode mode);
        WindowPositionMode trackingGetWinPosMode(TargetWindow windowSelect);
        System::Void trackingSetWindowSize(TargetWindow windowSelect, System::UInt16 width, System::UInt16 height);
        System::Void trackingGetWindowSize(TargetWindow windowSelect, System::UInt16 %width, System::UInt16 %height);
        System::Void trackingSetWindowWidth(TargetWindow windowSelect, System::UInt16 width);
        System::UInt16 trackingGetWindowWidth(TargetWindow windowSelect);
        System::Void trackingSetWindowHeight(TargetWindow windowSelect, System::UInt16 height);
        System::UInt16 trackingGetWindowHeight(TargetWindow windowSelect);
        System::Void trackingSetWindowPosX(TargetWindow windowSelect, System::UInt16 x);
        System::Void trackingSetWindowPosY(TargetWindow windowSelect, System::UInt16 y);
        System::Void trackingSetWindowPos(TargetWindow windowSelect, System::UInt16 x, System::UInt16 y);
        System::Void trackingGetWindowPos(TargetWindow windowSelect, System::UInt16 %x, System::UInt16 %y);
        System::Void trackingSetWindowColorEn(TargetWindow windowSelect, System::Boolean r_en, System::Boolean g_en, System::Boolean b_en);
        System::Void trackingSetWindowColorEnR(TargetWindow windowSelect, System::Boolean r_en);
        System::Void trackingSetWindowColorEnG(TargetWindow windowSelect, System::Boolean g_en);
        System::Void trackingSetWindowColorEnB(TargetWindow windowSelect, System::Boolean b_en);
        System::Void trackingGetWindowColorEn(TargetWindow windowSelect, System::Boolean %r_en, System::Boolean %g_en, System::Boolean %b_en);
        System::Void trackingSetWindowMaskImage(TargetWindow windowSelect, System::Int32 mask_img);
        System::Int32 trackingGetWindowMaskImage(TargetWindow windowSelect);
        System::Void trackingSetWinAllMask0(System::Void);
        System::Void trackingSetWinAllMask1(System::Void);
        System::Void trackingSetMcPixSize(System::Int32 width, System::Int32 height);
        System::Void trackingSetObjSizeMin(TargetWindow windowSelect, System::UInt32 min);
        System::UInt32 trackingGetObjSizeMin(TargetWindow windowSelect);
        System::Void trackingSetMvMin(TargetWindow windowSelect, System::UInt32 min);
        System::UInt32 trackingGetMvMin(TargetWindow windowSelect);
        System::Void trackingSetMvMax(TargetWindow windowSelect, System::UInt32 max);
        System::UInt32 trackingGetMvMax(TargetWindow windowSelect);
        System::Void trackingSetWindowDispCondition(TargetWindow windowSelect, WindowDisplayParam %par);
        System::Void trackingGetWindowDispCondition(TargetWindow windowSelect, WindowDisplayParam %par);
        System::Void trackingSetWindowDispConditionAnd(TargetWindow windowSelect, System::Boolean useSize, System::Boolean useMvMgn, System::Boolean useMvDir);
        System::Void trackingGetWindowDispConditionAnd(TargetWindow windowSelect, System::Boolean %useSize, System::Boolean %useMvMgn, System::Boolean %useMvDir);
        System::Void trackingSetWindowDispConditionOr(TargetWindow windowSelect, System::Boolean useSize, System::Boolean useMvMgn, System::Boolean useMvDir);
        System::Void trackingGetWindowDispConditionOr(TargetWindow windowSelect, System::Boolean %useSize, System::Boolean %useMvMgn, System::Boolean %useMvDir);
        System::Void trackingSetColorDetectEn(System::Boolean en);
        System::Boolean trackingGetColorDetectEn(System::Void);
        System::Void trackingSetColorDetectMode(ColorDetectMode mode);
        ColorDetectMode trackingGetColorDetectMode(System::Void);
        System::Void trackingSetColorDetectRatio(ColorDetectRatio %par);
        System::Void trackingGetColorDetectRatio(ColorDetectRatio %par);
        System::Void trackingSetColorDetectBase(ColorDetectBase base);
        System::Void trackingSetColorDetectAlpha(System::Single alpha);
        System::Void trackingSetColorDetectBeta(System::Single beta);
        System::Void trackingSetColorDetectGammaMin(System::Byte gammaMin);
        System::Void trackingSetColorDetectGammaMax(System::Byte gammaMax);
        System::Void trackingSetColorDetectRgbMaxMin(ColorDetectMaxMin %par);
        System::Void trackingGetColorDetectRgbMaxMin(ColorDetectMaxMin %par);
        System::Void trackingSetMonoTrackingMin(System::Byte mono_min);
        System::Void trackingSetMonoTrackingMax(System::Byte mono_max);
        System::Void trackingSetMonoTrackingParam(System::Byte min, System::Byte max);
        System::Void trackingGetMonoTrackingParam(System::Byte %min, System::Byte %max);
        System::Void trackingGetResult(System::Int32 %result, System::Int32 %moment0, System::Int32 %grav_x, System::Int32 %grav_y);
        System::Void trackingSetFwHold(System::Boolean en);
        System::Boolean trackingGetFwHold(System::Void);
        System::Void trackingSetWindowCtrl(TargetWindow windowSelect, System::Boolean en);
        System::Boolean trackingGetWindowCtrl(TargetWindow windowSelect);
        System::Void trackingSetWindowOverlapCtrl(TargetWindow windowSelect, System::Boolean en);
        System::Boolean trackingGetWindowOverlapCtrl(TargetWindow windowSelect);
        System::Void trackingSetWindowRatio(System::Single ratio);
        System::Single trackingGetWindowRatio(System::Void);
        System::Void trackingSetWindowMin(System::UInt16 min);
        System::UInt16 trackingGetWindowMin(System::Void);
        System::Void trackingSetWindowFrame(System::UInt32 line_width, System::UInt32 r, System::UInt32 g, System::UInt32 b);
        System::Void trackingSetTargetMode(TargetMode target_mode);
        System::Void trackingSetSearchPos(TargetWindow windowSelect, System::UInt16 x, System::UInt16 y);
        System::Void trackingGetSearchPos(TargetWindow windowSelect, System::UInt16 %x, System::UInt16 %y);
        System::Void trackingSetSearchPosX(TargetWindow windowSelect, System::UInt16 x);
        System::UInt16 trackingGetSearchPosX(TargetWindow windowSelect);
        System::Void trackingSetSearchPosY(TargetWindow windowSelect, System::UInt16 y);
        System::UInt16 trackingGetSearchPosY(TargetWindow windowSelect);
        System::Void trackingSetSearchSize(TargetWindow windowSelect, System::UInt16 width, System::UInt16 height);
        System::Void trackingGetSearchSize(TargetWindow windowSelect, System::UInt16 %width, System::UInt16 %height);
        System::Void trackingSetSearchWidth(TargetWindow windowSelect, System::UInt16 width);
        System::UInt16 trackingGetSearchWidth(TargetWindow windowSelect);
        System::Void trackingSetSearchHeight(TargetWindow windowSelect, System::UInt16 height);
        System::UInt16 trackingGetSearchHeight(TargetWindow windowSelect);
        System::Void trackingSetWHRatio(TargetWindow windowSelect, System::Single ratio);
        System::Single trackingGetWHRatio(TargetWindow windowSelect);
        System::Void trackingSetSearchPosRatio(TargetWindow windowSelect, System::Single centerXRatio, System::Single centerYRatio);
        System::Void trackingGetSearchPosRatio(TargetWindow windowSelect, System::Single %centerXRatio, System::Single %centerYRatio);
        System::Void trackingSetSearchPosRatioX(TargetWindow windowSelect, System::Single centerXRatio);
        System::Single trackingGetSearchPosRatioX(TargetWindow windowSelect);
        System::Void trackingSetSearchPosRatioY(TargetWindow windowSelect, System::Single centerYRatio);
        System::Single trackingGetSearchPosRatioY(TargetWindow windowSelect);
        System::Void trackingSetSearchSizeRatio(TargetWindow windowSelect, System::Single widthRatio, System::Single heightRatio);
        System::Void trackingGetSearchSizeRatio(TargetWindow windowSelect, System::Single %widthRatio, System::Single %heightRatio);
        System::Void trackingSetSearchWidthRatio(TargetWindow windowSelect, System::Single widthRatio);
        System::Single trackingGetSearchWidthRatio(TargetWindow windowSelect);
        System::Void trackingSetSearchHeightRatio(TargetWindow windowSelect, System::Single heightRatio);
        System::Single trackingGetSearchHeightRatio(TargetWindow windowSelect);
        System::Void trackingSetOutputImageSelect(OutputImageSelect outputImageSelect);
        OutputImageSelect trackingGetOutputImageSelect(System::Void);

        enum class SiOutMode : int
        {
            Old,
            New
        };

        enum class SiOutCsMode : int
        {
            Little,
            Big
        };

        System::Void siOutSetup(System::String ^fileName);
        System::Void siOutSetMode(SiOutMode mode);
        SiOutMode siOutGetMode(System::Void);
        System::Void siOutSetCsMode(SiOutCsMode endian);
        SiOutCsMode siOutGetCsMode(System::Void);
        System::Void siOutSetHFlip(System::Boolean en);
        System::Boolean siOutGetHFlip(System::Void);
        System::Void siOutSetVFlip(System::Boolean en);
        System::Boolean siOutGetVFlip(System::Void);
        System::Void siOutSetScalingFactor(System::Single scalingFactor);
        System::Single siOutGetScalingFactor(System::Void);
        System::Void siOutSetNum(System::Byte num);
        System::Byte siOutGetNum(System::Void);
        System::Void siOutSetFramePeriod(System::Byte framePeriod);
        System::Byte siOutGetFramePeriod(System::Void);
        System::Void siOutSetImageWidth(System::UInt16 width);
        System::UInt16 siOutGetImageWidth(System::Void);
        System::Void siOutSetImageHeight(System::UInt16 height);
        System::UInt16 siOutGetImageHeight(System::Void);

		// other
		void setGpuEn(System::Boolean en);
		System::Boolean getGpuEn(void);
		System::Int64 getTimeRawStreamMicroSec(void);
		System::Single getTimeRawStreamFrameRate(void);
        System::Void setSavePacketEn(System::Boolean en);
        System::UInt64 getTotalFrameCount(System::Void);
        System::UInt64 getLostFrameCount(System::Void);

        System::Byte readSensRegU8(System::UInt32 addr);
        System::UInt16 readSensRegU16(System::UInt32 addr);
        System::UInt32 readSensRegU32(System::UInt32 addr);
        System::UInt32 readSensRegU32(System::UInt32 addr, System::UInt32 msb, System::UInt32 lsb);
        System::Void writeSensRegU8(System::UInt32 addr, System::Byte val);
        System::Void writeSensRegU16(System::UInt32 addr, System::UInt16 val);
        System::Void writeSensRegU32(System::UInt32 addr, System::UInt32 val);
        System::Void writeSensRegU32(System::UInt32 addr, System::UInt32 msb, System::UInt32 lsb, System::UInt32 val);
        System::Byte readFpgaRegU8(System::UInt32 addr);
        System::UInt16 readFpgaRegU16(System::UInt32 addr);
        System::UInt32 readFpgaRegU32(System::UInt32 addr);
        System::UInt32 readFpgaRegU32(System::UInt32 addr, System::UInt32 msb, System::UInt32 lsb);
        System::Void writeFpgaRegU8(System::UInt32 addr, System::Byte val);
        System::Void writeFpgaRegU16(System::UInt32 addr, System::UInt16 val);
        System::Void writeFpgaRegU32(System::UInt32 addr, System::UInt32 val);
        System::Void writeFpgaRegU32(System::UInt32 addr, System::UInt32 msb, System::UInt32 lsb, System::UInt32 val);

        ref struct DriverInfo
        {
            System::String ^dll_ver;
            System::String ^sys_ver;
            System::String ^firmware_ver;
        };
        System::Void getDriverInfo(DriverInfo %info);

    private:
        HsvCam *m_camera;

	};
}
