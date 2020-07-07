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
		namespace stream {

class IStream
{
public:

	struct DeviceIdentify {
		std::string	device_name;
		std::string	device_id;
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
	 * Initialize.
	 *
	 * @param[in] packet_pixel_num		Number of packet pixel.
	 * @param[in] packet_frame_size		Packet frame size.
	 * @param[in] callback				Callback API when acquiring capture.
	 */
	virtual Status_t init(int32_t packet_pixel_num, int32_t packet_frame_size, const CallBack_t &callback) = 0;

	/**
	 * Get activate device list.
	 *
	 * @param[out] device_list		active device name list.
	 */
	virtual void getDeviceList(std::vector<DeviceIdentify>& device_list) = 0;

	/**
	 * Open stream.
	 *
	 * @param[in] device_name	open device name.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	virtual bool open(std::string device_name) = 0;

	/**
	 * Close stream.
	 */
	virtual void close() = 0;

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
};

		}
	}
}