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

class IHsvComponent
{
public:

	struct DeviceIdentify {
		std::string	device_name;
		std::string	device_id;
	};

	struct InitParam {
		DeviceIdentify device;
		int32_t packet_pixel_num;
		int32_t packet_frame_size;
		std::string port_name;
		uint32_t baudrate;
	};

	enum Status_t
	{
		E_STATUS_SUCCESS,
		E_STATUS_ERR_DEVICE_NOT_FOUND,
		E_STATUS_ERR_FAILED_TO_INITIALIZE,
		E_STATUS_ERR_PARAM_INCORRECT,
		E_STATUS_ERR_FAILED_TO_ALLOCATE,
		E_STATUS_ERR_DEVICE_ERROR
	};

	typedef std::function<void(const uint8_t *pPacket, int32_t packetSize)> CallBack_t;

	/**
	 * Bind callback.
	 *
	 * @param[in] func		Callback API.
	 * @param[in] owner		Callback destination instance.
	 *
	 * @return callback structure.
	 */
	template<typename T>
	static CallBack_t bindCallback(void (T::*func)(const uint8_t *pPacket, int32_t packetSize), T *owner)
	{
		return std::bind(func, owner, std::placeholders::_1, std::placeholders::_2);
	};


	/**
	 * Get activate device list.
	 *
	 * @param[out] device_list		active device name list.
	 */
//	virtual void getDeviceList(std::vector<DeviceIdentify>& device_list) = 0;

	/**
	 * Initialize component.
	 *
	 * @param[in] param		Initialize parameter.
	 * @param[in] callback	Callback API when acquiring capture.
	 *
	 * @return status.
	 */
	virtual Status_t initComponent(const InitParam& param, const CallBack_t &callback) = 0;

	/**
	 * Open stream.
	 *
	 * @param[in] device_name	open device name.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	virtual bool open() = 0;

	/**
	 * Close stream.
	 */
	virtual bool close() = 0;

	/**
	 * Get frame size(width & height).
	 *
	 * @param[out] width	frame width.
	 * @param[out] height	frame height.
	 */
	virtual void getPacketSize(int &width, int &height) = 0;

	// -----------------------------------
	// Asyncronous process
	// -----------------------------------
	/**
	 * Start capture.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	virtual bool startCapture() = 0;

	/**
	 * Stop capture.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	virtual bool stopCapture() = 0;

	// -----------------------------------
	// Syncronous process
	// -----------------------------------
	virtual bool start() = 0;

	/**
	 * Get current frame data.
	 *
	 * @param[out]	buff		receive frame data buffer.
	 * @param[in]	buff_size	buffer size.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	virtual bool getCurrentFrame(uint8_t* buff, uint32_t buff_size) = 0;

	virtual bool stop() = 0;










	/**
	 * @enum	State Code.
	 * @brief	Definition status code.
	 */
	enum StateCode : uint32_t {
		SUCCESS = (0x00000000),	///< Success.
		NOT_ENOUGH_MEMORY = (0x00000001),	///< Insufficient memory space.
		OUTOFMEMORY = (0x00000002),	///< Insufficient free memory.
		NOT_READY = (0x00000003),	///< Processing incomplete or Processing.
		INVALID_PARAMETER = (0x00000004),	///< Invalid parameter.
		BUSY = (0x00000005),	///< Device Busy.
		IO_DEVICE = (0x00000006),	///< Device I/O Error.
		DEVICE_NOT_CONNECTED = (0x00000007),	///< Device not connected.
		TIMEOUT = (0x00000008),	///< Process timeout.
		DEVICE_IN_USE = (0x00000009),	///< Device in use.
		DEVICE_NOT_AVAILABLE = (0x0000000A),	///< Device unavailable.
		RECV_ERR_CODE = (0x00001001),	///< Receive Error code form Sensor.
		SEQ_NO_ERR = (0x00001002),	///< Sequence Number Error.
		COMMUNICATE_ERR = (0x00001003),	///< Communicate error with Sensor.
		CHECKSUM_ERR = (0x00001004),	///< Checksum Error.
		ERROR_OTHER = (0xFFFFFFFF),	///< Other Error.
	};

	/**
	 * @enum	Response Code.
	 * @brief	IMX382 response code.
	 */
	enum ResponseCode : uint8_t {
		// Success
		ST_SUCCESS = 0x01,			///< Success
		// Command error
		ST_ERR_CMD_CMD = 0xF1,	///< [Command]Command Error.
		ST_ERR_CMD_CATEGORY = 0xF2,	///< [Command]Category No Error.
		ST_ERR_CMD_OFFSET = 0xF3,	///< [Command]Offset Error.
		ST_ERR_CMD_AREA = 0xF4,	///< [Command]Invalid Area Access Error.
		ST_ERR_CMD_CMD_LEN = 0xF7,	///< [Command]Command Length Error.
		ST_ERR_CMD_FLASH = 0xFA,	///< [Command]Flash Access Error.
		// Packet error
		ST_ERR_PAC_BYTE = 0xF0,	///< [Packet]Number of communicate bytes Error.
		ST_ERR_PAC_CMDNUM = 0xF5,	///< [Packet]Number of Commands Error.
		ST_ERR_PAC_CHECKSUM = 0xF6, ///< [Packet]Check Sum Error.
		ST_ERR_PAC_COMM = 0xFC,	///< [Packet]Communicate Error.
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


	virtual uint8_t readSensRegU8(uint32_t addr) = 0;
	virtual uint16_t readSensRegU16(uint32_t addr) = 0;
	virtual uint32_t readSensRegU32(uint32_t addr) = 0;
	virtual uint32_t readSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb) = 0;

	virtual void writeSensRegU8(uint32_t addr, uint8_t val) = 0;
	virtual void writeSensRegU16(uint32_t addr, uint16_t val) = 0;
	virtual void writeSensRegU32(uint32_t addr, uint32_t val) = 0;
	virtual void writeSensRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val) = 0;

	virtual uint8_t readFpgaRegU8(uint32_t addr) = 0;
	virtual uint16_t readFpgaRegU16(uint32_t addr) = 0;
	virtual uint32_t readFpgaRegU32(uint32_t addr) = 0;
	virtual uint32_t readFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb) = 0;

	virtual void writeFpgaRegU8(uint32_t addr, uint8_t val) = 0;
	virtual void writeFpgaRegU16(uint32_t addr, uint16_t val) = 0;
	virtual void writeFpgaRegU32(uint32_t addr, uint32_t val) = 0;
	virtual void writeFpgaRegU32(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t val) = 0;


	virtual StateCode writeSensReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size) = 0;
	virtual StateCode readSensReg(uint8_t category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size) = 0;
	virtual StateCode unlockSensFlash() = 0;
	virtual StateCode allWriteSensFlash() = 0;
	virtual StateCode eraseSensFlash(uint32_t addr) = 0;
	virtual StateCode writeSensFlash(uint32_t addr, uint8_t write_data[], int write_size) = 0;
	virtual StateCode readSensFlash(uint32_t addr, uint8_t* read_buff_ptr, int read_size) = 0;

	virtual StateCode writeFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size) = 0;
	virtual StateCode readFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size) = 0;
	virtual StateCode unlockFpgaFlash() = 0;
	virtual StateCode allWriteFpgaFlash() = 0;
	virtual StateCode eraseFpgaFlash(uint32_t addr) = 0;
	virtual StateCode writeFpgaFlash(uint32_t addr, uint8_t write_data[], int write_size) = 0;
	virtual StateCode readFpgaFlash(uint32_t addr, uint8_t* read_buff_ptr, int read_size) = 0;

	virtual StateCode rebootSens(void) = 0;
	virtual StateCode rebootFpga(void) = 0;

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


    struct DriverInfo
    {
        std::string dll_ver;
        std::string sys_ver;
        std::string firmware_ver;
    };
    virtual StateCode getDriverInfo(DriverInfo &info) = 0;

};

	}
}