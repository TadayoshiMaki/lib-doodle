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

#include "ICommand.h"

namespace so {
	namespace sdkdrv {
		namespace serial {
			class Serial;
		}
	}
}

namespace so {
	namespace sdkdrv {
		namespace command {

class CommandSsp : public ICommand {

public:

	/**
	 * Constructor.
	 */
	CommandSsp();

	/**
	 * Destructor.
	 */
	virtual ~CommandSsp();

	/**
	 * Open serial port.
	 *
	 * @param[in] port_name	Serial Port Name.
	 * @param[in] speed		baud rate.
	 *
	 * @retnum	true	open success.
	 * @retnum	false	open failed.
	 */
	bool open(const char port_name[], uint32_t baudrate);

	/**
	 * Close Serial port.
	 */
	bool close();

	/**
	 * Write register(Camera).
	 *
	 * @param[in] category		category.
	 * @param[in] address_ofset	offset.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeCameraRegister(uint8_t category, uint16_t address_ofset, uint8_t write_data[], WriteByteSize write_size);

	/**
	 * Write register(Micom).
	 *
	 * @param[in] category		category.
	 * @param[in] address_ofset	offset.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeMicomRegister(uint8_t category, uint16_t address_ofset, uint8_t write_data[], WriteByteSize write_size);


	/**
	 * Read register(Camera).
	 *
	 * @param[in] category		category.
	 * @param[in] address_ofset	offset.
	 * @param[in] read_buff_ptr	read data.
	 * @param[in] read_size		read data size.
	 *
	 * @return	result process.
	 */
	StateCode readCameraRegister(uint8_t category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);

	/**
	 * Read register(Micom).
	 * 
	 * @param[in] category		category.
	 * @param[in] address_ofset	offset.
	 * @param[in] read_buff_ptr	read data.
	 * @param[in] read_size		read data size.
	 *
	 * @return	result process.
	 */
	StateCode readMicomRegister(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);


	/**
	 * Write Flash(Camera).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeCameraFlash(uint32_t address, uint8_t write_data[], int write_size);

	/**
	 * Write Flash(Micom).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeMicomFlash(uint32_t address, uint8_t write_data[], int write_size);


	/**
	 * Read Flash data(Camera).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	StateCode readCameraFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);

	/**
	 * Read Flash data(Micom).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	StateCode readMicomFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);


	/**
	 * Unlock Flash(Camera).
	 *
	 * @return	result process.
	 */
	StateCode unlockCameraFlash();
	
	/**
	 * Unlock Flash(Micom).
	 *
	 * @return	result process.
	 */
	StateCode unlockMicomFlash();

	/**
	 * All Register Data write to Flash(Camera).
	 *
	 * @return	result process.
	 */
	StateCode allWriteCameraFlash();

	/**
	 * All Register Data write to Flash(Micom).
	 *
	 * @return	result process.
	 */
	StateCode allWriteMicomFlash();

	/**
	 * Erase Flash(Camera).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	StateCode eraseCameraFlash(uint32_t address);

	/**
	 * Erase Flash(Micom).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	StateCode eraseMicomFlash(uint32_t address);

	/**
	 * Reboot(Camera).
	 *
	 * @return	result process.
	 */
	StateCode rebootCamera(void);

	/**
	 * Start SDK Log.
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	StateCode startLog(LogMode mode);

	/**
	 * Stop SDK Log.
	 *
	 * @return	result process.
	 */
	StateCode stopLog();

	/**
	 * Get SDK Log.
	 *
	 * @param[in] index			Log acquisition start position.
	 * @param[in] number		Number of Log acquisitions.
	 *
	 * @return	result process.
	 */
	StateCode getLog(uint8_t index, uint8_t number);

	uint8_t getLastResponseError() { return m_last_err; }

private:

	/**
	 * Log data structure.
	 */
	struct LogData {
		uint32_t time;
		uint32_t log_no;
		uint32_t param;
	};

	uint8_t	m_seq_no;
	uint8_t	m_last_err;

	CommandSsp::StateCode writeI2C(uint32_t slaveAddr, uint32_t apbAddr, uint8_t *buf_ptr, uint32_t num);
	CommandSsp::StateCode readI2C(uint32_t slaveAddr, uint32_t apbAddr, uint8_t *buf_ptr, uint32_t num, uint32_t timeout);
};

		}
	}
}