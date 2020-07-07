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

#include "so_def.h"

namespace so
{
	namespace component
	{
		class IHsvComponent;
	}
}

namespace so {
    namespace sensing {
        class Tracking
        {
		public:

			Tracking(so::component::IHsvComponent& component);
			~Tracking();

            void init(void);
            void run(void);
            void setWinDispEn(TargetWindow windowSelect, bool en);
            bool getWinDispEn(TargetWindow windowSelect);
            void setWinArea(TargetWindow windowSelect, WindowArea mode);
            WindowArea getWinArea(TargetWindow windowSelect);
            void setWinDispMode(TargetWindow windowSelect, WindowDisplayMode mode);
            WindowDisplayMode getWinDispMode(TargetWindow windowSelect);
            void setWinPosMode(TargetWindow windowSelect, WindowPositionMode mode);
            WindowPositionMode getWinPosMode(TargetWindow windowSelect);
            void setWindowSize(TargetWindow windowSelect, uint16_t width, uint16_t height);
            void getWindowSize(TargetWindow windowSelect, uint16_t &width, uint16_t &height);
            void setWindowWidth(TargetWindow windowSelect, uint16_t width);
            uint16_t getWindowWidth(TargetWindow windowSelect);
            void setWindowHeight(TargetWindow windowSelect, uint16_t height);
            uint16_t getWindowHeight(TargetWindow windowSelect);
            void setWindowPosX(TargetWindow windowSelect, uint16_t x);
            uint16_t setWindowPosY(TargetWindow windowSelect, uint16_t y);
            void setWindowPos(TargetWindow windowSelect, uint16_t x, uint16_t y);
            void getWindowPos(TargetWindow windowSelect, uint16_t &x, uint16_t &y);
            void setWindowColorEn(TargetWindow windowSelect, bool r_en, bool g_en, bool b_en);
            void setWindowColorEnR(TargetWindow windowSelect, bool r_en);
            void setWindowColorEnG(TargetWindow windowSelect, bool g_en);
            void setWindowColorEnB(TargetWindow windowSelect, bool b_en);
            void getWindowColorEn(TargetWindow windowSelect, bool &r_en, bool &g_en, bool &b_en);
            void setWindowMaskImage(TargetWindow windowSelect, int mask_img);
            int getWindowMaskImage(TargetWindow windowSelect);
            void setWinAllMask0(void);
            void setWinAllMask1(void);
            void setMcPixSize(int width, int height);
            void setObjSizeMin(TargetWindow windowSelect, uint32_t min);
            uint32_t getObjSizeMin(TargetWindow windowSelect);
            void setMvMin(TargetWindow windowSelect, uint32_t min);
            uint32_t getMvMin(TargetWindow windowSelect);
            void setMvMax(TargetWindow windowSelect, uint32_t max);
            uint32_t getMvMax(TargetWindow windowSelect);
            void setWindowDispCondition(TargetWindow windowSelect, const WindowDisplayParam &par);
            void getWindowDispCondition(TargetWindow windowSelect, WindowDisplayParam &par);
            void setWindowDispConditionAnd(TargetWindow windowSelect, bool useSize, bool useMvMgn, bool useMvDir);
            void getWindowDispConditionAnd(TargetWindow windowSelect, bool &useSize, bool &useMvMgn, bool &useMvDir);
            void setWindowDispConditionOr(TargetWindow windowSelect, bool useSize, bool useMvMgn, bool useMvDir);
            void getWindowDispConditionOr(TargetWindow windowSelect, bool &useSize, bool &useMvMgn, bool &useMvDir);
            void setColorDetectEn(bool en);
            bool getColorDetectEn(void);
            void setColorDetectMode(ColorDetectMode mode);
            ColorDetectMode getColorDetectMode(void);
            void setColorDetectRatio(const ColorDetectRatio &par);
            void getColorDetectRatio(ColorDetectRatio &par);
            void setColorDetectBase(ColorDetectBase base);
            void setColorDetectAlpha(float alpha);
            void setColorDetectBeta(float beta);
            void setColorDetectGammaMin(uint8_t gammaMin);
            void setColorDetectGammaMax(uint8_t gammaMax);
            void setColorDetectRgbMaxMin(const ColorDetectMaxMin &par);
            void getColorDetectRgbMaxMin(ColorDetectMaxMin &par);
            void setMonoTrackingMin(uint8_t mono_min);
            void setMonoTrackingMax(uint8_t mono_max);
            void setMonoTrackingParam(uint8_t min, uint8_t max);
            void getMonoTrackingParam(uint8_t &min, uint8_t &max);
            int convColorCoefFtoI(float f_param);
            float convColorCoefItoF(int i_param);
            void getResult(int &result, int &moment0, int &grav_x, int &grav_y);
            void setFwHold(bool en);
            bool getFwHold(void);
            void setWindowCtrl(TargetWindow windowSelect, bool en);
            bool getWindowCtrl(TargetWindow windowSelect);
            void setWindowOverlapCtrl(TargetWindow windowSelect, bool en);
            bool getWindowOverlapCtrl(TargetWindow windowSelect);
            void setWindowRatio(float ratio);
            float getWindowRatio(void);
            void setWindowWidthMin(uint16_t min);
            uint16_t getWindowWidthMin(void);
            void setWindowFrameForDisplay(uint32_t line_width, uint32_t r, uint32_t g, uint32_t b);
            void setTargetMode(TargetMode target_mode);
            void setSearchPos(TargetWindow windowSelect, uint16_t x, uint16_t y);
            void getSearchPos(TargetWindow windowSelect, uint16_t &x, uint16_t &y);
            void setSearchPosX(TargetWindow windowSelect, uint16_t x);
            uint16_t getSearchPosX(TargetWindow windowSelect);
            void setSearchPosY(TargetWindow windowSelect, uint16_t y);
            uint16_t getSearchPosY(TargetWindow windowSelect);
            void setSearchSize(TargetWindow windowSelect, uint16_t width, uint16_t height);
            void getSearchSize(TargetWindow windowSelect, uint16_t &width, uint16_t &height);
            void setSearchWidth(TargetWindow windowSelect, uint16_t width);
            uint16_t getSearchWidth(TargetWindow windowSelect);
            void setSearchHeight(TargetWindow windowSelect, uint16_t height);
            uint16_t getSearchHeight(TargetWindow windowSelect);

            void setObjectAreaCoeff(TargetWindow windowSelect, float widthRatio, float heightRatio);
            void getObjectAreaCoeff(TargetWindow windowSelect, float &widthRatio, float &heightRatio);
            void setOutputImageSelect(OutputImageSelect outputImageSelect);
            OutputImageSelect getOutputImageSelect(void);

            void setAluProg(std::string fileName);

		private:
            void setAluProgPre(void);
            void runAlu(void);

			so::component::IHsvComponent&	m_component;
		};
    }
}

