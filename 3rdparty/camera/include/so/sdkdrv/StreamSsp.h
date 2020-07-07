
#pragma once

#include <stdint.h>

#include "usb/usbclnt_api.h"

#include "IStream.h"

namespace so
{
	class usb_client;
}

namespace so {
	namespace sdkdrv {
		namespace stream {

class StreamSsp : public IStream
{
public:

	/**
	 * Constructor.
	 */
	StreamSsp();

	/**
	 * Destructor.
	 */
	~StreamSsp();

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
	struct FrameBuffer_t
	{
		p_usbfunc_frame_data_t p_frame_data;
	};
	struct PacketHeader_t
	{
		uint32_t magicNumber;
		uint16_t width;
		uint16_t height;
		uint32_t frameNumber;
	};

	bool openUSB(size_t pixelNum);
//	void savePacket(const std::string &fileName, const uint8_t *pPacket);
	p_usbfunc_frame_data_t allocFrameBuffer(size_t size);
	bool putFrameBuffer(p_usbfunc_frame_data_t p_frame_buffer);
	p_usbfunc_frame_data_t next(void);
	void CALLBACK updateFrameBuffer();
	void CALLBACK freeFrameBuffer(p_usbfunc_frame_data_t *pp_buffer_list, size_t num_buffers, void *p_arg);

//	void readSensReg(uint8_t category, uint16_t address_ofset, uint8_t *read_buff_ptr, int read_size);
//	void writeSensReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);
//	void readFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t *read_buff_ptr, int read_size);
//	void writeFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);

	bool m_isOpened;
	bool m_savePacketEn;
	int m_idx;
	p_usbfunc_frame_data_t *m_p_frame_buf;   // フレームバッファへのポインタ配列
	int32_t m_framePixelNum;
	int32_t m_frameDataSize;
	CallBack_t m_callBack;
	int m_device_id;
	usb_client *m_usb_client;

};

		}
	}
}