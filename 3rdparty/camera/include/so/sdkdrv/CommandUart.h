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
	namespace component {
		namespace serial {
			class Serial;
		}
	}
}

namespace so {
	namespace component {
		namespace command {

class CommandUart : public ICommand {

public:

	/**
	 * Constructor.
	 */
	CommandUart();

	/**
	 * Destructor.
	 */
	virtual ~CommandUart();

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
	StateCode writeSensReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);

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
	StateCode writeFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);


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
	StateCode readSensReg(uint8_t category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);

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
	StateCode readFpgaReg(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);


	/**
	 * Write Flash(Camera).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeSensFlash(uint32_t address, uint8_t write_data[], int write_size);

	/**
	 * Write Flash(Micom).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	StateCode writeFpgaFlash(uint32_t address, uint8_t write_data[], int write_size);


	/**
	 * Read Flash data(Camera).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	StateCode readSensFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);

	/**
	 * Read Flash data(Micom).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	StateCode readFpgaFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);


	/**
	 * Unlock Flash(Camera).
	 *
	 * @return	result process.
	 */
	StateCode unlockSensFlash();
	
	/**
	 * Unlock Flash(Micom).
	 *
	 * @return	result process.
	 */
	StateCode unlockFpgaFlash();

	/**
	 * All Register Data write to Flash(Camera).
	 *
	 * @return	result process.
	 */
	StateCode allWriteSensFlash();

	/**
	 * All Register Data write to Flash(Micom).
	 *
	 * @return	result process.
	 */
	StateCode allWriteFpgaFlash();

	/**
	 * Erase Flash(Camera).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	StateCode eraseSensFlash(uint32_t address);

	/**
	 * Erase Flash(Micom).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	StateCode eraseFpgaFlash(uint32_t address);

	/**
	 * Reboot(Camera).
	 *
	 * @return	result process.
	 */
	StateCode rebootSens(void);

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

	serial::Serial* m_serial;
	uint8_t	m_seq_no;
	uint8_t	m_last_err;

	StateCode writeRegister(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size, uint8_t dst_id);
	StateCode readRegister(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size, uint8_t dst_id);

	StateCode writeFlash(uint32_t address, uint8_t write_data[], int write_size, uint8_t dst_id);
	StateCode readFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size, uint8_t dst_id);

	StateCode unlockFlash(uint8_t dst_id);

	StateCode allWriteFlash(uint8_t dst_id);

	StateCode eraseFlash(uint32_t address, uint8_t dst_id);


	void setPacketHeader(uint8_t pkt_buff[], uint8_t dst_id, uint8_t pkt_size);

	uint8_t calcCheckSum(const uint8_t pkt_buff[], uint32_t calc_size);

	StateCode getResponseCode(const uint8_t pkt_buff[], uint32_t resp_size, uint32_t recv_size);

	void dumpData(const uint8_t data[], uint32_t data_size);
};

		}
	}
}