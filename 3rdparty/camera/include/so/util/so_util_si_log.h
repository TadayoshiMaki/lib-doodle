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

#include "so_def.h"

#include <vector>


namespace so { 
    namespace util {
        class SiLog {
        public:
            void enqueue(SensingInformationPacket_t &si);
            void save(const std::string &dirPath);
        private:
            std::vector<SensingInformationPacket_t> m_que;
        };
    }
}

