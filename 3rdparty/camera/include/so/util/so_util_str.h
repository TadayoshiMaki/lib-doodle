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

namespace so {
    namespace util {
        namespace str {
            bool isNumericDec(wchar_t c);
            bool isNumericHex(wchar_t c);
			std::string convertWchar2String(wchar_t* name);
        }
    }
}
