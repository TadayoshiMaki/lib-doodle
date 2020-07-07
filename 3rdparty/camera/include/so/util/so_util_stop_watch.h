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
#include <chrono>

namespace so {
    namespace util {
        class StopWatch
        {
        public:
            StopWatch();
            void restart(void);
            int64_t getElapsedTimeMilliSec(void);
            int64_t getElapsedTimeMicroSec(void);
            int64_t getElapsedTimeNanoSec(void);
        private:
            std::chrono::system_clock::time_point m_start;
        };
    }
}

