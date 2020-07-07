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
#include <array>

namespace so { 

	enum class SystemSelect
	{
		PiccoloX,
		SDK
	};

	enum class RegisterInterface
	{
		USB3,		///< to SSP
		UART,		///< to DBG B'd
		USB_UVC,	///< to UVC
	};

    enum class SensorMode : int {
        FULL_10BIT_0120FPS = 3,
        FULL_08BIT_0120FPS = 48,
        VH12_08BIT_0240FPS = 16,
        FULL_04BIT_0500FPS = 7,
        VH12_04BIT_1000FPS = 18,
    };

    enum
    {
        FORMAT_TABLE_NUM = 6,
        FORMAT_TABLE_SENSING_NUM = 3,
    };

	// so::sensing
	struct Position_t {
		uint16_t x;
		uint16_t y;
	};
	enum TargetWindow
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

	struct SensingInformation_t {
		bool result;
		uint32_t moment0;
		uint32_t moment1_x;
		uint32_t moment1_y;
		Position_t grvpos;
		Position_t ave_grvpos;
		Position_t mvpos;
		uint16_t mvmgn;
		uint8_t  mvdir;
	};

	struct SensingInformationPacket_t {
		uint32_t frameCount;
        std::array<SensingInformation_t, WinNum> win;
	};

	// so::sensing::Tracking
	enum WindowArea
	{
		All,
		Local
	};

	enum WindowDisplayMode
	{
		Always,
		Detected
	};

	enum class WindowPositionMode : uint8_t
	{
		Reg,
		GrvPos,
		AveGrvPos,
	};

	enum WindowDisplayCondition
	{
		Mode3And,
		Mode3Or,
		Mode2And,
		Mode2Or
	};

	struct WindowDisplayParam
	{
		WindowDisplayCondition mode;
		bool useSize;
		bool useMvMgn;
		bool useMvDir;
	};

	enum ColorDetectMode
	{
		MinMax,
		ColorRatio
	};

	enum ColorDetectBase
	{
		Red,
		Green,
		Blue,
	};
	struct ColorDetectRatio
	{
		ColorDetectBase base;
		float   alpha;
		float   beta;
		uint8_t gammaMin;
		uint8_t gammaMax;
	};
	struct ColorDetectMaxMin
	{
		uint32_t rMax;
		uint32_t rMin;
		uint32_t gMax;
		uint32_t gMin;
		uint32_t bMax;
		uint32_t bMin;
	};

	enum TargetMode
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

	// so::sensing::Common
	enum FdGain : int {
		Low,
		High
	};

	// so::fpga::Fpga
	struct SensingFirmwareVersion_t
	{
		int major;
		int minor;
		int buildNumber;
		int revision;
	};

	enum ResetConfig_t
	{
		E_RESET_CONFIG_0,
		E_RESET_CONFIG_1,
		E_RESET_CONFIG_2,
		E_RESET_CONFIG_3
	};

	enum MainStatus_t {
		E_STATUS_NONE,
		E_STATUS_REBOOT,
		E_STATUS_SENSING_CTRL
	};

	enum DepthMode_t {
		E_DEPTH_1 = 1,
		E_DEPTH_4 = 4,
		E_DEPTH_8 = 8,
		E_DEPTH_10 = 10,
		E_DEPTH_12 = 12
	};

	enum MitopUnpackMode_t {
		E_UNPACKMODE_RAW10 = 0,
		E_UNPACKMODE_RAW08 = 1,
		E_UNPACKMODE_RAW12 = 4,
		E_UNPACKMODE_RAW14 = 5
	};

	enum class GpifSizeSel : int {
		Auto,
		Manual
	};

	struct Config_t {
		SensorMode mode;
		ResetConfig_t resetConfig;
		DepthMode_t depth;
		uint16_t pixelWidth;
		uint16_t pixelHeight;
		uint16_t visibleWidth;
		uint16_t visibleHeight;
		MitopUnpackMode_t unpackMode;
		uint8_t validShift;
		bool del1Line;
		bool halfHpix;
		uint8_t lvdsCalibration;
		GpifSizeSel gpifSizeSel;
		uint16_t gpifManualHPix;
		uint16_t gpifManualVLine;
	};

	enum class SiOutMode : int {
		Old,
		New
	};

	enum class SiOutCsMode : int
	{
		On,
		LowFixed,
		HighFixed
	};

	// so::imgproc
	struct Param_t
	{
		int  rgb_conv_mode;
		bool draw_color_detect_bin;
	};
}

