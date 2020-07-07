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
#include <stdint.h>

#include "usbclnt_api.h"

namespace so {
class usb_client
{
public:
    WINAPI usb_client();
    WINAPI usb_client(int device_id);
    WINAPI ~usb_client();

    typedef std::function<void(void)> CallBackUpdateFrame;
    template<typename T1>
    CallBackUpdateFrame bindCallBackUpdateFrame(void (CALLBACK T1::*func)(void), T1 *owner)
    {
        return std::bind(func, owner);
    };

    typedef std::function<void(p_usbfunc_frame_data_t *pp_buffer_list, size_t num_buffers)> CallBackFreeFrameBuffer;
    template<typename T>
    CallBackFreeFrameBuffer bindCallBackFreeFrameBuffer(void (T::*func)(p_usbfunc_frame_data_t *pp_buffer_list, size_t num_buffers), T *owner)
    {
        return std::bind(func, owner, std::placeholders::_1, std::placeholders::_2);
    };

	void init(int device_id);

    DWORD WINAPI open(
        size_t num_pixels,
        CallBackFreeFrameBuffer &callBack,
        void *p_arg,
        DWORD transfer_length,
        BYTE num_pending_reads,
        usbclnt_transfer_mode_t transfer_mode,
        bool debug_mode_on
    );

    void WINAPI close(void);
    DWORD WINAPI start_capture(CallBackUpdateFrame &callBack);
    DWORD WINAPI stop_capture(void);
    DWORD WINAPI put_frame_buffer(p_usbfunc_frame_data_t p_frame_buffer);
    DWORD WINAPI get_frame_data(p_usbfunc_frame_data_t *pp_frame_data,
        size_t *p_num_pixels);
    size_t WINAPI get_frame_buffer_size(void);
    DWORD WINAPI get_version(const char **pp_dll_ver,
        const char **pp_sys_ver);
   void WINAPI log_msg(const char *p_string);
   void WINAPI set_log_mask(unsigned int mask);
    // FX3 I2C
    DWORD WINAPI get_firm_version(uint32_t *p_firm_version);
    DWORD WINAPI fx3_put_chars(const char *p_buf, uint32_t num);
    DWORD WINAPI fx3_get_chars(char *p_buf);
    DWORD WINAPI i2c_write(uint32_t slaveAddr, uint32_t apbAddr, uint8_t *p_buf, uint32_t num);
    DWORD WINAPI i2c_read(uint32_t slaveAddr, uint32_t apbAddr, uint8_t *p_buf, uint32_t num, uint32_t timeout);

    enum board_reset_kind
    {
        BOARD_RESET_I2C = 0, //!< I2C Block in The FPGA
        BOARD_RESET_FPGA = 1, //!< FPGA All
    };
    DWORD WINAPI board_reset(uint32_t kind);
    // FX3 I2C
    // USB2.0HS
    DWORD WINAPI board_ping(p_usbfunc_board_ping_status_t ping_status);
    // USB2.0HS
    // GET VIDEO PROPERTY
    DWORD WINAPI get_video_property(p_usbfunc_video_property_t p_property, uint32_t timeout);
    void WINAPI cancel_get_video_property(void);

    static void CALLBACK updateFrameBuffer(DWORD result, void *p_arg);
    void CALLBACK _updateFrameBuffer(DWORD result);

    static void CALLBACK freeFrameBuffer(p_usbfunc_frame_data_t *pp_buffer_list, size_t num_buffers, void *p_arg);
    void CALLBACK _freeFrameBuffer(p_usbfunc_frame_data_t *pp_buffer_list, size_t num_buffers);

    void readSensReg(uint8_t category, uint16_t address_ofset, uint8_t *read_buff_ptr, int read_size);
    void writeSensReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);
    void readFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t *read_buff_ptr, int read_size);
    void writeFpgaReg(uint8_t category, uint16_t address_ofset, uint8_t write_data[], int write_size);

    DWORD reboot(void);

	void unlockSensFlash();
	void allWriteSensFlash();
	void eraseSensFlash(uint32_t address);
	void writeSensFlash(uint32_t address, uint8_t write_data[], int write_size);
	void readSensFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);

	void unlockFpgaFlash();
	void allWriteFpgaFlash();
	void eraseFpgaFlash(uint32_t address);
	void writeFpgaFlash(uint32_t address, uint8_t write_data[], int write_size);
	void readFpgaFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);

private:
    int m_device_id;

    enum {
        DEVICE_NUM_MAX = 2
    };

    CallBackUpdateFrame m_updateFrame;
    static CallBackFreeFrameBuffer m_freeFrameBuffer;

};
}



