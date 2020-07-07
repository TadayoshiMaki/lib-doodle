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
        class Reg {
        public:

			Reg(so::sdkdrv::command::ICommand& sdk_command);
			~Reg() {};

			uint8_t readU8(uint32_t addr);
            uint16_t readU16(uint32_t addr);
            uint32_t readU32(uint32_t addr);
            void writeU8(uint32_t addr, uint8_t val);
            uint32_t readU32(uint32_t addr, uint32_t msb, uint32_t lsb);
            void writeU16(uint32_t addr, uint16_t val);
            void writeU32(uint32_t addr, uint32_t val);
            void read(uint8_t category, uint16_t address_ofset, uint8_t *read_buff_ptr, int read_size);
            void write(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);
            void writeU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val);
        private:
			so::sdkdrv::command::ICommand& m_sdk_command;
        };
    }
}
