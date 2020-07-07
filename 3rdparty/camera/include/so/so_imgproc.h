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
#include <opencv2/opencv.hpp>

namespace so { 
    namespace imgproc {
        enum {
            IDX_B,
            IDX_G,
            IDX_R,
            COLOR_NUM
        };

		// unused
        //enum E_WB_SEL {
        //    E_WB_MANUAL,
        //    E_WB_AUTO
        //};

    }
}

#include "imgproc/so_color_def.h"
#include "imgproc/so_imgproc_whitebalance.h"
#include "imgproc/so_imgproc_rec.h"
#include "imgproc/so_imgproc_log.h"
#include "imgproc/so_imgproc_util.h"
#include "imgproc/so_imgproc_viewer.h"
