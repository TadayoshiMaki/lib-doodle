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

#define SI_LOG_NUM_MAX (1000)

#include <opencv2/opencv.hpp>

#include "so_def.h"

namespace so { 
    namespace imgproc {
        class viewer
        {
        public:
			viewer();
			virtual ~viewer();

            using USER_PROC_t = void(*)(cv::InputArray img_src, cv::OutputArray img_dst, const SensingInformationPacket_t &si, const Param_t &par);

			void init();
            void setUserProc(USER_PROC_t user_proc);
            void setWbGain(float gain_r, float gain_g, float gain_b);
            void setWbGainR(float gain_r);
            void setWbGainG(float gain_g);
            void setWbGainB(float gain_b);
            void resetSi(bool en);

            void setIspEn(bool en);
            bool getIspEn(void);
            void setDrawSiEn(TargetWindow windowSelect, bool en);
            bool getDrawSiEn(TargetWindow windowSelect);
            void setDrawGravPosEn(bool en);
            bool getDrawGravPosEn(void);
            void setDrawVectorEn(bool en);
            bool getDrawVectorEn(void);
            void setDrawDetectAreaEn(bool en);
            bool getDrawDetectAreaEn(void);
            void setDrawTrajectoryEn(bool en);
            bool getDrawTrajectoryEn(void);
            void setDrawMoment0En(bool en);
            bool getDrawMoment0En(void);
            void setSiLogNum(int num);
            int getSiLogNum(void);
            void setDrawBinarizedImage(bool en);
            bool getDrawBinarizedImage(void);

            void setGravPosPointSize(int32_t size);
            void setTrajectoryPointSize(int32_t size);

            void setViewParam(const Param_t &par);

            void setSi(const SensingInformationPacket_t &si);
			void rollbackSi(const SensingInformationPacket_t &si);

            void convert(const cv::Mat &img_src, const SensingInformationPacket_t &si, cv::Mat &img_dst, bool hFlip, bool vFlip);
            void bayer2rgb(const cv::Mat &imgRaw, cv::OutputArray imgRgb, bool hFlip, bool vFlip);

            void drawSi(cv::InputOutputArray img, TargetWindow windowSelect, const SensingInformationPacket_t &si);
            void drawMoment(cv::InputOutputArray img, TargetWindow windowSelect, const SensingInformationPacket_t &si, const Position_t &pos);


            void setColorDetectEn(bool en);
            bool getColorDetectEn(void);
            void setColorDetectMode(ColorDetectMode mode);
            ColorDetectMode getColorDetectMode(void);
            void setColorDetectRatio(const ColorDetectRatio &par);
            void getColorDetectRatio(ColorDetectRatio &par);
            void setColorDetectRgbMaxMin(const ColorDetectMaxMin &par);
            void getColorDetectRgbMaxMin(ColorDetectMaxMin &par);

            void setCropEn(bool en);
            bool getCropEn(void);
            void setCrop(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
            void setCropOffsetX(uint32_t x);
            void setCropOffsetY(uint32_t y);
            void setCropWidth(uint32_t width);
            void setCropHeight(uint32_t height);
            void getCrop(uint32_t &x, uint32_t &y, uint32_t &width, uint32_t &height);
            uint32_t getCropOffsetX(void);
            uint32_t getCropOffsetY(void);
            uint32_t getCropWidth(void);
            uint32_t getCropHeight(void);
            void crop(cv::InputArray src, cv::OutputArray dst, uint32_t offsetX, uint32_t offsetY, uint32_t width, uint32_t height);

            void setObjectAreaCoeff(TargetWindow windowSelect, float widthCoeff, float heightCoeff);

            void colorDetect(cv::InputArray imgSrc, cv::OutputArray imgBin);

        private:
			void resetSiAuto(const SensingInformationPacket_t &si);
			void drawGravPos(cv::InputOutputArray img, const Position_t& grvpos, const cv::Scalar &color);
            void drawVector(cv::InputOutputArray img, const Position_t& grvpos, uint8_t mvdir, uint16_t mvmgn, const cv::Scalar &color);
            void drawTrajectory(cv::InputOutputArray img, TargetWindow windowSelect, const cv::Scalar &color);
            void binarizeColorRatio(cv::InputArray _img_src, cv::OutputArray _img_dst, const ColorDetectRatio &par);
            void binarizeColorMaxMin(cv::InputArray _img_src, cv::OutputArray _img_dst, const ColorDetectMaxMin &par);
            uint32_t limitCropWidth(uint32_t cropOffsetX, uint32_t visibleWidth, uint32_t orgWidth);
            uint32_t limitCropHeight(uint32_t cropOffsetY, uint32_t visibleHeight, uint32_t orgHeight);

            enum StartPixel_t {
                R,
                Gr,
                Gb,
                B
            };
            StartPixel_t selectStartPixel(uint32_t width, uint32_t height, bool hFlip, bool vFlip);

            bool m_is_opened;
            bool m_isp_en;
            bool m_drawSi_en[WinNum];
            bool m_drawGravPos_en;
            bool m_drawVector_en;
            bool m_draw_detect_area_en;
            bool m_draw_trajectory_en;
            bool m_drawMoment0_en;

            bool m_resetSi;
            int32_t m_gravPosPointSize;
            int32_t m_trajectoryPointSize;
            USER_PROC_t m_userProc;
            float m_gain_r;
            float m_gain_g;
            float m_gain_b;
            int m_si_log_num;
            SensingInformationPacket_t m_si_latest;
            SensingInformationPacket_t m_si[SI_LOG_NUM_MAX]; // TODO vectorに変更
            int m_si_log_cnt;
            Param_t m_view_par;
            bool m_color_detect_en;
            ColorDetectMode m_color_detect_mode;
            ColorDetectRatio m_color_detect_ratio;
            ColorDetectMaxMin m_color_detect_rgb_maxmin;
            int m_resetSi_cnt[WinNum];
            bool m_cropEn;
            uint32_t m_cropOffsetX;
            uint32_t m_cropOffsetY;
            uint32_t m_cropWidth;
            uint32_t m_cropHeight;

            struct ObjectAreaCoeff
            {
                float width;
                float height;
            };
            ObjectAreaCoeff m_objectAreaCoeff[WinNum];

        };
    }
}

