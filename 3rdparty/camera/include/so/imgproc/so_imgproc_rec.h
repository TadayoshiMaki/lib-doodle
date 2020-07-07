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

#include "so_def.h"

#define REC_FPS (60.0f)

namespace so { 
    namespace imgproc {
        class viewer;


        class rec {
        public:
            enum E_STILL_CODEC_t {
                E_STILL_CODEC_JPG,
                E_STILL_CODEC_PNG,
                E_STILL_CODEC_BMP
            };
            enum E_MOVIE_CODEC_t {
                E_MOVIE_CODEC_MP4,
                E_MOVIE_CODEC_DIB
            };

            rec(viewer* view);
            ~rec();

            void enqueue(const cv::Mat &img, const so::SensingInformationPacket_t &si);
            void enqurePreFrame(const cv::Mat &img, const so::SensingInformationPacket_t &si, int frm_num_pre);
            void saveRaw(std::string path);
            void saveStill(std::string path, bool drawSi, E_STILL_CODEC_t codec, bool hFlip, bool vFlip);
            void saveMovie(std::string fileName, float frameRate, int skip, const std::string &post_fix, bool drawSi, E_MOVIE_CODEC_t codec, bool hFlip, bool vFlip);
            void saveSiLog(std::string path, bool hFlip, bool vFlip);
            int getFrameNum(void);
            bool getImage(int idx, cv::Mat &img, so::SensingInformationPacket_t &si, bool hFlip, bool vFlip);
            void clear(void);

        private:
            struct ST_REC_DATA_t {
                cv::Mat img;
                so::SensingInformationPacket_t si;
            };

            std::vector<ST_REC_DATA_t> m_que_pre;
            std::vector<ST_REC_DATA_t> m_que;

            viewer *m_isp;
        };
    }
}

