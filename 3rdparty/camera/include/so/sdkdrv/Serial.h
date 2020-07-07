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
	namespace component {
		namespace serial {

class Serial {

public:

	enum Byte : uint8_t {
		BYTE7 = 7,
		BYTE8 = 8,
	};

	enum Parity : uint8_t {
		NO_PARITY = 0,
		ODD_PARITY = 1,
		EVEN_PARITY = 2,
		MARK_PARITY = 3,
		SPACE_PARITY = 4,
	};

	enum StopBits : uint8_t {
		ONE = 0,
		ONE5 = 1,
		TWO = 2,
	};

	/**
	 * Constructor.
	 */
	Serial();

	/**
	 * Destructor.
	 */
	~Serial();

	/**
	 * Connect Serial port.
	 *
	 * @param[in] port_name	Serial Port Name.
	 * @param[in] speed		baud rate.
	 * @param[in] bits		number of bits.
	 * @param[in] parity	parity.
	 * @param[in] stop_bits	stopbits.
	 *
	 * @retnum	true	open success.
	 * @retnum	false	open failed.
	 */
	bool connect(const char port_name[], uint32_t baudrate, Byte bits = BYTE8, Parity parity = NO_PARITY, StopBits stop_bits = ONE);

	/**
	 * Disconnect Serial port.
	 */
	bool disconnect();

	/**
	 * Write data.
	 *
	 * @param[in] write_data		write data.
	 * @param[in] write_size		write data size.
	 * @return	written byte size.
	 */
	virtual int32_t write(const uint8_t write_data[], uint32_t write_size);

	/**
	 * Read data.
	 *
	 * @param[out] read_data		read data.
	 * @param[in] read_size			read data size.
	 * @param[in] timeout			read time out[unit:ms].
	 * @return	read byte size.
	 */
	virtual int32_t read(uint8_t* read_data, uint32_t read_size, uint32_t timeout);

private:

	void* m_com_handle;

	virtual bool setReadTimeout(uint32_t time);
};

		}
	}
}
