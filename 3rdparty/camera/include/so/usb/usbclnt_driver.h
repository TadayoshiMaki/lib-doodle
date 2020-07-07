//==============================================================================
// Copyright (C) <2014> Sony Corp. All Rights Reserved.
//
// This program is proprietary and confidential. By using this program
// you agree to the terms of the associated Software License Agreement.
//------------------------------------------------------------------------------
//! @file
//! @brief  Internal interfaces of USB client software
//!         (usbclnt driver to usbclnt API).
//==============================================================================

#pragma once

#ifdef WIN32
#include <Windows.h>
#include <Tchar.h>
#include <INITGUID.H>
#else
#include <Ntddk.h>
//#include <Wdm.h>
#endif // WIN32

#include <Public.h>

//------------------------------------------------------------------------------
// define and structures
//------------------------------------------------------------------------------

// < Win32 data type name >
//   +-----------------+-------------------+
//   | user space name | kernel space name |
//   +=================+===================+
//   |     DWORD      <=>      ULONG       |
//   +-----------------+-------------------+
//   |     LPVOID     <=>      PVOID       |
//   +-----------------+-------------------+
//   |              ULONGLONG              |
//   +-----------------+-------------------+

/***********************************************************
 * System I/O control codes (IOCTL) for our usbclnt driver *
 ***********************************************************/
//! "DeviceType" value for "CTL_CODE()" \n
//! (0-32767(0x7FFF): reserved for Microsoft, 32768(0x8000)-65535(0xFFFF): reserved for OEMs and IHVs)
#define FILE_DEVICE_USBCLNT 0xF00D //!< "DeviceType" of our usbclnt driver

//! offset of "Function" values for "CTL_CODE()" \n
//! (0-2047(0x7FF): reserved for Microsoft, 2048(0x800)-4095(0xFFF): reserved for OEMs and IHVs)
#define IOCTL_INDEX         0x800 //!< "Function" values offset of our usbclnt driver

//! IOCTL - "open target function"
#define IOCTL_USBCLNT_OPEN                \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 0, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

//! IOCTL - "close target function"
#define IOCTL_USBCLNT_CLOSE               \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

//! IOCTL - "issue start capture command"
#define IOCTL_USBCLNT_START_CAPTURE       \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 2, METHOD_BUFFERED, FILE_WRITE_ACCESS))

//! IOCTL - "issue stop capture command"
#define IOCTL_USBCLNT_STOP_CAPTURE        \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 3, METHOD_BUFFERED, FILE_WRITE_ACCESS))

//! IOCTL - "put frame buffer"
#define IOCTL_USBCLNT_PUT_FRAME_BUFFER    \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 4, METHOD_BUFFERED, FILE_WRITE_ACCESS))

//! IOCTL - "get frame data"
#define IOCTL_USBCLNT_GET_FRAME_DATA      \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 5, METHOD_BUFFERED, FILE_READ_ACCESS))

//! IOCTL - "get MaxPacketSize"
#define IOCTL_USBCLNT_GET_MAX_PACKET_SIZE \
         ((ULONG) CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 6, METHOD_BUFFERED, FILE_READ_ACCESS))

/// FX3 I2C //////////////
//! IOCTL - "get firm ware version"
#define IOCTL_USBCLNT_GET_FIRM_VERSION ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 7, METHOD_BUFFERED, FILE_READ_ACCESS))

//! IOCTL - "Put charactors to FX3"
#define IOCTL_USBCLNT_FX3_PUT_CHARS    ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 8, METHOD_BUFFERED, FILE_WRITE_ACCESS))

//! IOCTL - "Get characters from FX3"
#define IOCTL_USBCLNT_FX3_GET_CHARS    ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 9, METHOD_BUFFERED, FILE_READ_ACCESS))

//! IOCTL - "Write the I2C data to FPGA via FX3"
#define IOCTL_USBCLNT_I2C_WRITE        ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 10, METHOD_BUFFERED, FILE_WRITE_ACCESS))

//! IOCTL - "Read the I2C data from FPGA via FX3"
#define IOCTL_USBCLNT_I2C_READ         ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 11, METHOD_BUFFERED, FILE_READ_ACCESS))

//! IOCTL - "Reset the specified block in FPGA via FX3"
#define IOCTL_USBCLNT_BOARD_RESET      ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 12, METHOD_BUFFERED, FILE_WRITE_ACCESS))
/// FX3 I2C //////////////

/// USB2.0HS /////////////
//! IOCTL - "Ping the board. (It is used for getting USB speed. and used for pinging the FX3.)"
#define IOCTL_USBCLNT_BOARD_PING         ((ULONG)CTL_CODE(FILE_DEVICE_USBCLNT, IOCTL_INDEX + 13, METHOD_BUFFERED, FILE_READ_ACCESS))
/// USB2.0HS /////////////

/******************************************
 * structure of each "InBuffer" for IOCTL *
 ******************************************/
//! "InBuffer" of "open target function"
#include <pshpack8.h>
typedef struct usbclnt_in_data_of_open {
	ULONGLONG ullFrameBufferSize; //!< frame buffer size (includes header and footer length)
	ULONG     dwTransferLength;   //!< Bulk-In transfer length ("TransferLength" for Continuous Reader)
	UCHAR     bNumPendingReads;	  //!< number of maximum pending operations for Bulk-In transfer ("NumPendingReads" for Continuous Reader)
} usbclnt_in_data_of_open_t, *p_usbclnt_in_data_of_open_t;
#include <poppack.h>

//! "InBuffer" of "put frame buffer"
#include <pshpack8.h>
typedef struct usbclnt_in_data_of_put_frame_buffer {
	ULONGLONG ullFrameBuffer; //!< address of the frame buffer
} usbclnt_in_data_of_put_frame_buffer_t, *p_usbclnt_in_data_of_put_frame_buffer_t;
#include <poppack.h>

/// FX3 I2C //////////////////////////////////////
//! "InBuffer" of "put charcters to FX3"
#define FX3_PUT_CHAR_BUFF_SIZE (256)
#include <pshpack4.h>
typedef struct {
	ULONG num;
	UCHAR Buffer[FX3_PUT_CHAR_BUFF_SIZE];
} usbclnt_in_data_of_fx3_put_chars_t, *p_usbclnt_in_data_of_fx3_put_chars_t;
#include <poppack.h>

//! "InBuffer" of "Write the I2C data to FPGA via FX3"
#define I2C_WRITE_BUFF_SIZE (sizeof(ULONG) + 256)
#define I2C_WRITE_SIZE_MAX  (255) /* This limitation is defined by FPGA I2C block's specification */
#include <pshpack4.h>
typedef struct {
	ULONG nWrite;                      //!< Number of write bytes.
	ULONG slaveAddr;                   //!< 7Bit I2C slave address. No shift LSB, The bits use BIT6 from BIT0.
	UCHAR Buffer[I2C_WRITE_BUFF_SIZE]; //!< "Internal 32Bit APB bus address in FPGA." + Write Data(256Byte)
} usbclnt_in_data_of_i2c_write_t, *p_usbclnt_in_data_of_i2c_write_t;
#include <poppack.h>

//! "InBuffer" of "Reset the specified block in FPGA via FX3"
#include <pshpack4.h>
typedef struct {
	/* The capture board handles 16GB of memories. */
	ULONG kind; //!< The specified block in the board.
} usbclnt_in_data_of_board_reset_t, *p_usbclnt_in_data_of_board_reset_t;
#include <poppack.h>
/// FX3 I2C //////////////////////////////////////

/*******************************************
 * structure of each "OutBuffer" for IOCTL *
 *******************************************/
//! "OutBuffer" of "get frame data"
#include <pshpack8.h>
typedef struct usbclnt_out_data_of_get_frame_data {
	ULONGLONG ullFrameBuffer;        //!< address of the frame buffer
	ULONGLONG ullBulkInReceivedSize; //!< amount of actual received size of Bulk-In transfers for the frame
} usbclnt_out_data_of_get_frame_data_t, *p_usbclnt_out_data_of_get_frame_data_t;
#include <poppack.h>

//! "OutBuffer" of "get MaxPacketSize"
#include <pshpack4.h>
typedef struct usbclnt_out_data_of_get_max_packet_size {
	ULONG dwBulkInMaxPacketSize; //!< "MaxPacketSize" of the Bulk-In pipe
} usbclnt_out_data_of_get_max_packet_size_t, *p_usbclnt_out_data_of_get_max_packet_size_t;
#include <poppack.h>

/// FX3 I2C //////////////////////////////////////
//! "OutBuffer" of "get firm ware version"
#include <pshpack4.h>
typedef struct usbclnt_out_data_of_get_firm_version {
	ULONG FirmwareVersion;
} usbclnt_out_data_of_get_firm_version_t, *p_usbclnt_out_data_of_get_firm_version_t;
#include <poppack.h>

//! "OutBuffer" of "get charactter from FX3"
#define FX3_GET_CHAR_BUFF_SIZE (256)
#include <pshpack4.h>
typedef struct {
	UCHAR Buffer[FX3_GET_CHAR_BUFF_SIZE];
} usbclnt_out_data_of_fx3_get_chars_t, *p_usbclnt_out_data_of_fx3_get_chars_t;
#include <poppack.h>
/// FX3 I2C //////////////////////////////////////

/// USB2.0HS /////////////////////////////////////
//! "OutBuffer" of "board ping"
#include <pshpack4.h>
typedef struct usbclnt_out_data_of_board_ping {
	ULONG UsbSpeed;
	ULONG PingStatus; // T.B.D
} usbclnt_out_data_of_board_ping_t, *p_usbclnt_out_data_of_board_ping_t;
#include <poppack.h>
/// USB2.0HS /////////////////////////////////////


/// FX3 I2C //////////////////////////////////////
/*********************************************
 * structure of each "InOutBuffer" for IOCTL *
 *********************************************/
//! "OutBuffer" of "Read the I2C data to FPGA via FX3"
#define I2C_READ_BUFF_SIZE (256)
#define I2C_READ_SIZE_MAX  (255) /* This limitation is defined by FPGA I2C block's specification */
#include <pshpack4.h>
typedef struct {
	IN  ULONG nRead ;                     //!< Number of read bytes.
	IN  ULONG slaveAddr;                  //!< 7Bit I2C slave address. No shift LSB, The bits use BIT6 from BIT0.
	IN  ULONG apbAddr32;                  //!< "Internal 32Bit APB bus address in FPGA."
	IN  ULONG timeout;                    //!< Timeout in msec
	OUT UCHAR Buffer[I2C_READ_BUFF_SIZE]; //!< Read Data
} usbclnt_inout_data_of_i2c_read_t, *p_usbclnt_inout_data_of_i2c_read_t;
#include <poppack.h>
/// FX3 I2C //////////////////////////////////////

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------


