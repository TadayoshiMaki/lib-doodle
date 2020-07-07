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

namespace so
{
	namespace sdkdrv
	{
		namespace command {
			class ICommand;
		}
	}
}

namespace so {
    namespace fpga {
        class Flash {
		public:
			Flash(so::sdkdrv::command::ICommand& sdk_command);
			~Flash();
			bool read(const std::string &fileName, int size);
            bool read(uint32_t flashAddress, uint8_t * buf, int size);
            bool read(uint8_t * buf, int size);
            bool write(std::string fileName, int size);

		private:
			so::sdkdrv::command::ICommand& m_sdk_command;
		};
    }
}
