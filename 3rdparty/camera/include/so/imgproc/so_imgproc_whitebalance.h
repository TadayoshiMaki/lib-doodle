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

namespace so { 
    namespace imgproc {
        class white_balance {
        public:
            static void gain(cv::InputArray _img_src, cv::OutputArray _img_dst, float gain_r, float gain_g, float gain_b);
            static void calcGainGreyWorld(cv::InputArray _img_src, float &gain_r, float &gain_g, float &gain_b);
            static float calcGain(int sum_rb, int sum_g);
        private:
            static uint16_t convWbGainFtoI(float f_gain);
            static float convWbGainItoF(uint16_t i_gain);
        };
    }
}

