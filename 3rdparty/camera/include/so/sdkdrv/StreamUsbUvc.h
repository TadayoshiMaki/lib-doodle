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
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "IStream.h"

namespace so {
	namespace component {
		namespace stream {

class StreamUsbUvc : public IStream, IMFSourceReaderCallback
{
public:

	/**
	 * Constructor.
	 */
	StreamUsbUvc();

	/**
	 * Destructor.
	 */
	~StreamUsbUvc();

	/**
	 * Initialize.
	 *
	 * @param[in] packet_pixel_num		Number of packet pixel.
	 * @param[in] packet_frame_size		Packet frame size.
	 * @param[in] callback				Callback API when acquiring capture.
	 */
	Status_t init(int32_t packet_pixel_num, int32_t packet_frame_size, const CallBack_t &callback);

	/**
	 * Get activate device list.
	 *
	 * @param[out] device_list		active device name list.
	 */
	void getDeviceList(std::vector<DeviceIdentify>& device_list);

	/**
	 * Open stream.
	 *
	 * @param[in] device_name	open device name.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool open(std::string device_name);

	/**
	 * Close stream.
	 */
	void close();

	/**
	 * Get frame size(width & height).
	 *
	 * @param[out] width	frame width.
	 * @param[out] height	frame height.
	 */
	void getPacketSize(int &width, int &height);

	// -----------------------------------
	// Asyncronous process
	// -----------------------------------
	/**
	 * Start capture.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool startCapture();

	/**
	 * Stop capture.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool stopCapture();

	// -----------------------------------
	// Syncronous process
	// -----------------------------------
	/**
	 * Start streaming.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool start();

	/**
	 * Get current frame data.
	 *
	 * @param[out]	buff		receive frame data buffer.
	 * @param[in]	buff_size	buffer size.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool getCurrentFrame(uint8_t* buff, uint32_t buff_size);

	/**
	 * Stop streaming.
	 *
	 * @retval true		success.
	 * @retval false	error.
	 */
	bool stop();

private:

	struct PacketHeader_t
	{
		uint32_t signature;		///< magic number
		uint16_t width;			///< frame width (number of pixels)
		uint16_t height;		///< frame height (number of pixels)
		uint32_t frame_number;	///< frame number
	};
	struct FrameBuffer_t
	{
		PacketHeader_t	header;		///< frame information header
		uint16_t*		pixel_data;	///< frame pixel data
	};
	struct PacketFooter_t {
		uint32_t signature;		///< magic number
		uint32_t frame_number;	///< frame number
		uint32_t reserved1;		///< Reserved1
		uint32_t reserved2;		///< Reserved2
	};

	IMFMediaSource* m_media_source;
	IMFSourceReader* m_source_reader;
	CallBack_t m_callback;
	int32_t m_frame_pixel_num;
	int32_t m_frame_data_size;
	uint32_t m_frame_number;

	CRITICAL_SECTION        m_critsec;
	long                    m_ref_count;        // Reference count.
	bool                    m_capture_flg;
	bool                    m_1st_sample_flg;
	LONGLONG                m_base_time;

	template<class T>
	void SafeRelease(T **pp) {
		try {
			if ((*pp) != nullptr) {
			(*pp)->Release();
			*pp = nullptr;
		};
		}
		catch (...) {

		}
	};

	IMFActivate ** getDeviceSource(UINT32& count);
	void releaseDeviceSource(IMFActivate **devices, const UINT32 count);

	bool validateFrameData(uint8_t* buff, int32_t buff_size);

	// IMFSourceReaderCallback methods
	HRESULT OnReadSample(HRESULT status, DWORD stream_index, DWORD stream_flags, LONGLONG timestamp, IMFSample* sample);

	HRESULT OnEvent(DWORD, IMFMediaEvent *)
	{
		return S_OK;
	}

	HRESULT OnFlush(DWORD)
	{
		return S_OK;
	}

	// IUnknown methods
	HRESULT QueryInterface(REFIID iid, void** ppv);

	ULONG AddRef();

	ULONG Release();

};

		}
	}
}