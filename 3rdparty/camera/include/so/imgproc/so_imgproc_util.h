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
    namespace imgproc {
        class util {
        public:
            static uint16_t convertHFlip(uint16_t x, uint16_t width);
            static uint16_t convertVflip(uint16_t y, uint16_t height);
            static bool bin2mat(const std::string &fileName, cv::Mat &img);
            static void flip(cv::InputArray _img_src, cv::OutputArray _img_dst, bool hflip, bool vflip);
            static void flip(SensingInformationPacket_t &si, bool hflip, bool vflip, uint16_t width, uint16_t height);
        };
    }
}
