//==============================================================================
// Copyright (C) <2014> Sony Corp. All Rights Reserved.
//
// This program is proprietary and confidential. By using this program
// you agree to the terms of the associated Software License Agreement.
//------------------------------------------------------------------------------
//! @file
//! @brief  Interfaces to USB client software (usbclnt API).
//==============================================================================

#pragma once

#include <Windows.h>
#include <stdio.h>

#include <usbfunc_spec.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//------------------------------------------------------------------------------
// define and structures
//------------------------------------------------------------------------------
//! transfer mode
typedef enum usbclnt_transfer_mode {
	USBCLNT_TRANSFER_MODE_NORMAL  = 0, //!< : normal mode
	USBCLNT_TRANSFER_MODE_PADDING = 1  //!< : padding mode
} usbclnt_transfer_mode_t, *p_usbclnt_transfer_mode_t;

//! debug mode
typedef enum usbclnt_debug_mode {
	USBCLNT_OPEN_MODE_NDEBUG  = false, //!< : normal mode
	USBCLNT_OPEN_MODE_DEBUG   = true   //!< : debug mode
} usbclnt_debug_mode_t, *p_usbclnt_debug_mode_t;


//! USB client software API header
#ifdef USBCLNT_API_EXPORTS
#define USBCLNT_IMPORT_EXPORT __declspec(dllexport)
#else
#define USBCLNT_IMPORT_EXPORT __declspec(dllimport)
#endif // USBCLNT_API_EXPORTS

//! function type of "discard frame buffers"
typedef void (CALLBACK *f_usbclnt_discard_frame_buffers_t)(p_usbfunc_frame_data_t *pp_buffer_list,
                                                           size_t num_buffers, void *p_arg);
//! function type of "frame capture completion"
typedef void (CALLBACK *f_usbclnt_frame_comp_t)(DWORD result, void *p_arg);

//! output the given format string to the log file and/or "cout"
#define USBCLNT_LOGMSG(p_format, ...)                                                \
          do {                                                                       \
            char str[300 + 1];                                                       \
            _snprintf_s(&(str[0]), _countof(str), _TRUNCATE, p_format, __VA_ARGS__); \
            usbclnt_log_msg(&(str[0]));                                              \
          } while (USBCLNT_FALSE)

//! Restriction mask of the output messages to logfile.
//! see, usbclnt_set_log_mask() and usbclnt_log_msg().
//! Originaly defination is into usbclnt_common.h.
#ifndef LOG_MASK_ERROR
	#define LOG_MASK_ERROR  (1<<0)
	#define LOG_MASK_INFO   (1<<1)
	#define LOG_MASK_DEBUG  (1<<2)
#endif // LOG_MASK_ERROR
#define USBCLNT_LOG_MASK_ERROR LOG_MASK_ERROR
#define USBCLNT_LOG_MASK_INFO  LOG_MASK_INFO
#define USBCLNT_LOG_MASK_DEBUG LOG_MASK_DEBUG

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------
extern USBCLNT_IMPORT_EXPORT const bool USBCLNT_FALSE;


//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_open(size_t num_pixels,
                                                f_usbclnt_discard_frame_buffers_t f_discard_frame_buffers,
                                                void *p_arg, DWORD transfer_length,
                                                BYTE num_pending_reads,
                                                usbclnt_transfer_mode_t transfer_mode,
                                                bool debug_mode_on);
USBCLNT_IMPORT_EXPORT void WINAPI usbclnt_close(void);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_start_capture(f_usbclnt_frame_comp_t f_frame_comp,
                                                         void *p_arg);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_stop_capture(void);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_put_frame_buffer(p_usbfunc_frame_data_t p_frame_buffer);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_get_frame_data(p_usbfunc_frame_data_t *pp_frame_data,
                                                          size_t *p_num_pixels);
USBCLNT_IMPORT_EXPORT size_t WINAPI usbclnt_get_frame_buffer_size(void);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_get_version(const char **pp_dll_ver,
                                                       const char **pp_sys_ver);
USBCLNT_IMPORT_EXPORT void WINAPI usbclnt_log_msg(const char *p_string);
USBCLNT_IMPORT_EXPORT void WINAPI usbclnt_set_log_mask(unsigned int mask);
// FX3 I2C
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_get_firm_version(uint32_t *p_firm_version);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_fx3_put_chars(const char *p_buf, uint32_t num);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_fx3_get_chars(char *p_buf);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_i2c_write(uint32_t slaveAddr,  uint32_t apbAddr, uint8_t *p_buf, uint32_t num);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_i2c_read (uint32_t slaveAddr,  uint32_t apbAddr, uint8_t *p_buf, uint32_t num, uint32_t timeout);

enum usbclnt_board_reset_kind {
	USBCLNT_BOARD_RESET_I2C  = 0, //!< I2C Block in The FPGA
	USBCLNT_BOARD_RESET_FPGA = 1, //!< FPGA All
};
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_board_reset(uint32_t kind);
// FX3 I2C
// USB2.0HS
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_board_ping(p_usbfunc_board_ping_status_t ping_status);
// USB2.0HS
// GET VIDEO PROPERTY
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_get_video_property(p_usbfunc_video_property_t p_property, uint32_t timeout);
USBCLNT_IMPORT_EXPORT  void WINAPI usbclnt_cancel_get_video_property(void);
// GET VIDEO PROPERTY

// Multi Device

USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_open(uint32_t device_instance,
	size_t num_pixels,
	f_usbclnt_discard_frame_buffers_t f_discard_frame_buffers,
	void *p_arg, DWORD transfer_length,
	BYTE num_pending_reads,
	usbclnt_transfer_mode_t transfer_mode,
	bool debug_mode_on);
USBCLNT_IMPORT_EXPORT void WINAPI musbclnt_close(uint32_t device_instance);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_start_capture(uint32_t device_instance,
	f_usbclnt_frame_comp_t f_frame_comp,
	void *p_arg);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_stop_capture(uint32_t device_instance);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_put_frame_buffer(uint32_t device_instance, p_usbfunc_frame_data_t p_frame_buffer);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_get_frame_data(uint32_t device_instance, p_usbfunc_frame_data_t *pp_frame_data,
	size_t *p_num_pixels);
USBCLNT_IMPORT_EXPORT size_t WINAPI musbclnt_get_frame_buffer_size(uint32_t device_instance);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_get_firm_version(uint32_t device_instance, uint32_t *p_firm_version);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_fx3_put_chars(uint32_t device_instance, const char *p_buf, uint32_t num);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_fx3_get_chars(uint32_t device_instance, char *p_buf);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_i2c_write(uint32_t device_instance, uint32_t slaveAddr, uint32_t apbAddr, uint8_t *p_buf, uint32_t num);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_i2c_read(uint32_t device_instance, uint32_t slaveAddr, uint32_t apbAddr, uint8_t *p_buf, uint32_t num, uint32_t timeout);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_board_reset(uint32_t device_instance, uint32_t kind);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_board_ping(uint32_t device_instance, p_usbfunc_board_ping_status_t ping_status);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_get_video_property(uint32_t device_instance, p_usbfunc_video_property_t p_property, uint32_t timeout);
USBCLNT_IMPORT_EXPORT  void WINAPI musbclnt_cancel_get_video_property(uint32_t device_instance);
USBCLNT_IMPORT_EXPORT DWORD WINAPI usbclnt_get_device_count(uint32_t *p_num);
USBCLNT_IMPORT_EXPORT DWORD WINAPI musbclnt_get_device_path(uint32_t device_instance, char *p_device_path, uint32_t size);

//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//! @brief Get string of the API result.
//
//! @param[in] val : Result value
//
//! @return Pointer to the string
//------------------------------------------------------------------------------
static inline const char *usbclnt_result_str(DWORD val)
{
	switch (val) {
	case ERROR_SUCCESS:              return "ERROR_SUCCESS";              break; //    0 (0x00000000)
	case ERROR_NOT_ENOUGH_MEMORY:    return "ERROR_NOT_ENOUGH_MEMORY";    break; //    8 (0x00000008)
	case ERROR_OUTOFMEMORY:          return "ERROR_OUTOFMEMORY";          break; //   14 (0x0000000E)
	case ERROR_NOT_READY:            return "ERROR_NOT_READY";            break; //   21 (0x00000015)
	case ERROR_INVALID_PARAMETER:    return "ERROR_INVALID_PARAMETER";    break; //   87 (0x00000057)
	case ERROR_BUSY:                 return "ERROR_BUSY";                 break; //  170 (0x000000AA)
	case ERROR_IO_DEVICE:            return "ERROR_IO_DEVICE";            break; // 1117 (0x0000045D)
	case ERROR_DEVICE_NOT_CONNECTED: return "ERROR_DEVICE_NOT_CONNECTED"; break; // 1167 (0x0000048F)
	case ERROR_TIMEOUT:              return "ERROR_TIMEOUT";              break; // 1460 (0x000005B4)
	case ERROR_DEVICE_IN_USE:        return "ERROR_DEVICE_IN_USE";        break; // 2404 (0x00000964)
	case ERROR_DEVICE_NOT_AVAILABLE: return "ERROR_DEVICE_NOT_AVAILABLE"; break; // 4319 (0x000010DF)
	default:                         return "Unknown";                    break;
	}
}

#ifdef __cplusplus
}
#endif // __cplusplus
