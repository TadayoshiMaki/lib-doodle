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

#include <functional>
#include <vector>
#include <string>
#include <stdint.h>

namespace so {
	namespace component {
		namespace command {

class ICommand
{
public:

	/**
		* @enum	State Code.
		* @brief	Definition status code.
		*/
	enum StateCode : uint32_t {
		SUCCESS					= (0x00000000),	///< Success.
		NOT_ENOUGH_MEMORY		= (0x00000001),	///< Insufficient memory space.
		OUTOFMEMORY				= (0x00000002),	///< Insufficient free memory.
		NOT_READY				= (0x00000003),	///< Processing incomplete or Processing.
		INVALID_PARAMETER		= (0x00000004),	///< Invalid parameter.
		BUSY					= (0x00000005),	///< Device Busy.
		IO_DEVICE				= (0x00000006),	///< Device I/O Error.
		DEVICE_NOT_CONNECTED	= (0x00000007),	///< Device not connected.
		TIMEOUT					= (0x00000008),	///< Process timeout.
		DEVICE_IN_USE			= (0x00000009),	///< Device in use.
		DEVICE_NOT_AVAILABLE	= (0x0000000A),	///< Device unavailable.
		RECV_ERR_CODE			= (0x00001001),	///< Receive Error code form Sensor.
		SEQ_NO_ERR				= (0x00001002),	///< Sequence Number Error.
		COMMUNICATE_ERR			= (0x00001003),	///< Communicate error with Sensor.
		CHECKSUM_ERR			= (0x00001004),	///< Checksum Error.
		ERROR_OTHER				= (0xFFFFFFFF),	///< Other Error.
	};

	/**
	 * @enum	Response Code.
	 * @brief	IMX382 response code.
	 */
	enum ResponseCode : uint8_t {
		// Success
		ST_SUCCESS = 0x01,			///< Success
		// Command error
		ST_ERR_CMD_CMD		= 0xF1,	///< [Command]Command Error.
		ST_ERR_CMD_CATEGORY = 0xF2,	///< [Command]Category No Error.
		ST_ERR_CMD_OFFSET	= 0xF3,	///< [Command]Offset Error.
		ST_ERR_CMD_AREA		= 0xF4,	///< [Command]Invalid Area Access Error.
		ST_ERR_CMD_CMD_LEN	= 0xF7,	///< [Command]Command Length Error.
		ST_ERR_CMD_FLASH	= 0xFA,	///< [Command]Flash Access Error.
		// Packet error
		ST_ERR_PAC_BYTE		= 0xF0,	///< [Packet]Number of communicate bytes Error.
		ST_ERR_PAC_CMDNUM	= 0xF5,	///< [Packet]Number of Commands Error.
		ST_ERR_PAC_CHECKSUM = 0xF6, ///< [Packet]Check Sum Error.
		ST_ERR_PAC_COMM		= 0xFC,	///< [Packet]Communicate Error.
		// error occurred in MICOM
		ST_ERR_MICOM_DST = 0xE0,	///< Dest ID Error.
		// Other error
		ST_ERR_OTHER = 0xFF,		///< Other error.
	};

	/**
	 * @enum	Log mode.
	 * @brief	SDK LogMode.
	 */
	enum LogMode : uint8_t {
		ROTATE = 0,     ///< If the maximum number of logs are saved, overwrite the oldest log
		MAX_STOP = 1,   ///< If the maximum number of logs are saved, do not store logs
		ERR_STOP = 2,   ///< If an error occurs, stop log store
	};

	/**
	 * Open serial port.
	 *
	 * @param[in] port_name	Serial Port Name.
	 * @param[in] speed		baud rate.
	 *
	 * @retnum	true	open success.
	 * @retnum	false	open failed.
	 */
	virtual bool open(const char port_name[], uint32_t baudrate) = 0;

	/**
	 * Close Serial port.
	 */
	virtual bool close() = 0;

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
	virtual StateCode writeSensReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size) = 0;

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
	virtual StateCode writeFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size) = 0;


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
	virtual StateCode readSensReg(uint8_t category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size) = 0;

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
	virtual StateCode readFpgaReg(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size) = 0;


	/**
	 * Write Flash(Camera).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	virtual StateCode writeSensFlash(uint32_t address, uint8_t write_data[], int write_size) = 0;

	/**
	 * Write Flash(Micom).
	 *
	 * @param[in] address		Flash address.
	 * @param[in] write_data	write data.
	 * @param[in] write_size	write data size.
	 *
	 * @return	result process.
	 */
	virtual StateCode writeFpgaFlash(uint32_t address, uint8_t write_data[], int write_size) = 0;


	/**
	 * Read Flash data(Camera).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	virtual StateCode readSensFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size) = 0;

	/**
	 * Read Flash data(Micom).
	 *
	 * @param[in] address			Flash address.
	 * @param[out] read_buff_ptr	read data.
	 * @param[in] read_size			read data size.
	 *
	 * @return	result process.
	 */
	virtual StateCode readFpgaFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size) = 0;


	/**
	 * Unlock Flash(Camera).
	 *
	 * @return	result process.
	 */
	virtual StateCode unlockSensFlash() = 0;

	/**
	 * Unlock Flash(Micom).
	 *
	 * @return	result process.
	 */
	virtual StateCode unlockFpgaFlash() = 0;

	/**
	 * All Register Data write to Flash(Camera).
	 *
	 * @return	result process.
	 */
	virtual StateCode allWriteSensFlash() = 0;

	/**
	 * All Register Data write to Flash(Micom).
	 *
	 * @return	result process.
	 */
	virtual StateCode allWriteFpgaFlash() = 0;

	/**
	 * Erase Flash(Camera).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	virtual StateCode eraseSensFlash(uint32_t address) = 0;

	/**
	 * Erase Flash(Micom).
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	virtual StateCode eraseFpgaFlash(uint32_t address) = 0;

	/**
	 * Reboot(Camera).
	 *
	 * @return	result process.
	 */
	virtual StateCode rebootSens(void) = 0;

	/**
	 * Start SDK Log.
	 *
	 * @param[in] address			Flash address.
	 *
	 * @return	result process.
	 */
	virtual StateCode startLog(LogMode mode) = 0;

	/**
	 * Stop SDK Log.
	 *
	 * @return	result process.
	 */
	virtual StateCode stopLog() = 0;

	/**
	 * Get SDK Log.
	 *
	 * @param[in] index			Log acquisition start position.
	 * @param[in] number		Number of Log acquisitions.
	 *
	 * @return	result process.
	 */
	virtual StateCode getLog(uint8_t index, uint8_t number) = 0;

};

		}
	}
}
