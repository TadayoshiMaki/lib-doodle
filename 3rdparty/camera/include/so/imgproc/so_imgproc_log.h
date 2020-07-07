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

namespace so { 
    namespace imgproc {
        class log {
        public:
            void enqueue(so::SensingInformationPacket_t &si);
            void save(std::string fileName);
        private:
            std::vector<so::SensingInformationPacket_t> m_que;
        };
    }
}

