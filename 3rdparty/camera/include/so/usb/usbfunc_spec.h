//==============================================================================
// Copyright (C) <2014> Sony Corp. All Rights Reserved.
//
// This program is proprietary and confidential. By using this program
// you agree to the terms of the associated Software License Agreement.
//------------------------------------------------------------------------------
//! @file
//! @brief  Definitions of USB function's specification. (vendor-specific)
//==============================================================================

#pragma once

#include <std_types.h>

//------------------------------------------------------------------------------
// define and structures
//------------------------------------------------------------------------------
/***********************
 * USB vendor requests *
 ***********************/
//! Request Codes for Vendor Request ("bRequest" value)
#define USBFUNC_RC_START_CAPTURE (0x00) //!< "Start Capture" request
#define USBFUNC_RC_STOP_CAPTURE  (0x01) //!< "Stop Capture" request
/// FX3 I2C ///
#define USBFUNC_RC_GET_FIRM_VERSION         (0x02) //!< "get firm ware version" request
#define USBFUNC_RC_FX3_PUT_CHARS            (0x03) //!< "FX3 put char(s)" request
#define USBFUNC_RC_FX3_GET_CHARS            (0x04) //!< "FX3 get char(s)" request
#define USBFUNC_RC_I2C_WRITE                (0x05) //!< "I2C Write" request
#define USBFUNC_RC_I2C_WRITE_GET_LAST_ERROR (0x06) //!< "I2C Write get last error" request
#define USBFUNC_RC_I2C_READ_SET_APB_ADDR    (0x07) //!< "I2C Read, Set 32Bit APB address" request
#define USBFUNC_RC_I2C_READ                 (0x08) //!< "I2C READ" request
#define USBFUNC_RC_BOARD_RESET              (0x09) //!< "Reset specified block in FPGA" request
#define USBFUNC_RC_BOARD_PING               (0x0A) //!< "Ping the board" request (getting the USB Speed)
/// FX3 I2C ///

/**************
 * frame data *
 **************/
//! transfer block size
#define USBFUNC_TRANSFER_BLOCK_SIZE (16 * 1024) //!< 16 KB

//! "frame information header"
#if defined(WIN32) || defined(__WINSYS)
#include <pshpack4.h>
#endif // defined(WIN32) || defined(__WINSYS)
typedef struct usbfunc_frame_header {
	uint32_t dwSignature;   //!< magic number
	uint16_t wWidth;        //!< frame width (number of pixels)
	uint16_t wHeight;       //!< frame height (number of pixels)
	uint32_t dwFrameNumber; //!< frame number
}
#if !defined(WIN32) && !defined(__WINSYS)
  __attribute__ ((packed,aligned(4)))
#endif // !defined(WIN32) && !defined(__WINSYS)
  usbfunc_frame_header_t, *p_usbfunc_frame_header_t;
#if defined(WIN32) || defined(__WINSYS)
#include <poppack.h>
#endif // defined(WIN32) || defined(__WINSYS)

//! magic number
#define USBFUNC_FRAME_SIGNATURE 0x594E4F53 //!< little endian of 'S' 'O' 'N' 'Y'

//! "frame information footer"
#if defined(WIN32) || defined(__WINSYS)
#include <pshpack4.h>
#endif // defined(WIN32) || defined(__WINSYS)
typedef struct usbfunc_frame_footer {
	uint32_t dwSignature;   //!< magic number
	uint32_t dwFrameNumber; //!< frame number
	uint32_t dwReserved1;   //!< Reserved1
	uint32_t dwReserved2;   //!< Reserved2
}
#if !defined(WIN32) && !defined(__WINSYS)
  __attribute__ ((packed,aligned(4)))
#endif // !defined(WIN32) && !defined(__WINSYS)
  usbfunc_frame_footer_t, *p_usbfunc_frame_footer_t;
#if defined(WIN32) || defined(__WINSYS)
#include <poppack.h>
#endif // defined(WIN32) || defined(__WINSYS)

//! magic number for footer
#define USBFUNC_FRAME_FOOTER_SIGNATURE       (0xA6B1B0AC)  //!< bit inversed 'S' 'O' 'N' 'Y'
//! Adds the bytes for align to permit odd height. (to escape from 1kb(max_paket_size) boundary.
#define USBFUNC_FRAME_FOOTER_ALIGN_SIZE      (4)          //!< add 4 bytes garbage.
#define USBFUNC_FRAME_FOOTER_ALIGN           (0x4E474C41) //!< little endian of 'A' 'L' 'G' 'N'

//! "frame data" (frame footer included)
#if defined(WIN32) || defined(__WINSYS)
#include <pshpack2.h>
#endif // defined(WIN32) || defined(__WINSYS)
typedef struct usbfunc_frame_data {
	usbfunc_frame_header_t header;       //!< frame information header
	uint16_t               *pixel_data;  //!< frame pixel data
}
#if !defined(WIN32) && !defined(__WINSYS)
  __attribute__ ((packed,aligned(2)))
#endif // !defined(WIN32) && !defined(__WINSYS)
  usbfunc_frame_data_t, *p_usbfunc_frame_data_t;
#if defined(WIN32) || defined(__WINSYS)
#include <poppack.h>
#endif // defined(WIN32) || defined(__WINSYS)

/// USB2.0HS /////////////////////////////////////
//! "ping staus" (USB Speed and FX3 status)
#if defined(WIN32) || defined(__WINSYS)
#include <pshpack4.h>
#endif // defined(WIN32) || defined(__WINSYS)
typedef struct usbfuc_board_ping_status {
#define USBFUNC_BOARD_PING_USB_SPEED_SS (0)         //!< FX3 Connect with USB3.0SS
#define USBFUNC_BOARD_PING_USB_SPEED_HS (1)         //!< FX3 Connect with USB2.0HS
	ULONG UsbSpeed;

	ULONG PingStatus; // T.B.D
}
#if !defined(WIN32) && !defined(__WINSYS)
  __attribute__ ((packed,aligned(4)))
#endif // !defined(WIN32) && !defined(__WINSYS)
  usbfunc_board_ping_status_t, *p_usbfunc_board_ping_status_t;
#if defined(WIN32) || defined(__WINSYS)
#include <poppack.h>
#endif // defined(WIN32) || defined(__WINSYS)
/// USB2.0HS /////////////////////////////////////

/// GET VIDEO PROPERTY ///////////////////////////
//! "video property" HV size
#if defined(WIN32) || defined(__WINSYS)
#include <pshpack4.h>
#endif // defined(WIN32) || defined(__WINSYS)
typedef struct usbfunc_video_property {
	ULONG Width;
	ULONG Height;
}
#if !defined(WIN32) && !defined(__WINSYS)
  __attribute__ ((packed,aligned(4)))
#endif // !defined(WIN32) && !defined(__WINSYS)
  usbfunc_video_property_t, *p_usbfunc_video_property_t;
#if defined(WIN32) || defined(__WINSYS)
#include <poppack.h>
#endif // defined(WIN32) || defined(__WINSYS)
/// GET VIDEO PROPERTY ///////////////////////////


//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------


