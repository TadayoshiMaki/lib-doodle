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

// ------------------------------
// SdkDriver Wrapper
// ------------------------------

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @enum	SDKError
 * @brief	エラー定義
 */
enum SDKError : uint32_t {
	SUCCESS				= (0x00000000),	/**< 正常動作 */
	NOT_ENOUGH_MEMORY	= (0x00000001),	/**< メモリー領域不足 */
	OUTOFMEMORY			= (0x00000002),	/**< 空きメモリー不足 */
	NOT_READY			= (0x00000003),	/**< 処理中,処理未完了 */
	INVALID_PARAMETER	= (0x00000004),	/**< 無効なパラメータ */
	BUSY				= (0x00000005),	/**< BUSY */
	IO_DEVICE			= (0x00000006),	/**< IOデバイスエラー */
	DEVICE_NOT_CONNECTED = (0x00000007),	/**< デバイス未接続 */
	TIMEOUT				= (0x00000008),	/**< タイムアウト */
	DEVICE_IN_USE		= (0x00000009),	/**< デバイス使用中 */
	DEVICE_NOT_AVAILABLE = (0x0000000A),	/**< デバイス使用不可 */
	RECV_ERR_CODE		= (0x00001001),
	SEQ_NO_ERR			= (0x00001002),
	COMMUNICATE_ERR		= (0x00001003),
	CHECKSUM_ERR		= (0x00001004),
	ERROR_OTHER			= (0xFFFFFFFF),	/**< その他のエラー */
};

enum Interface : int {
	USB3,	/// to FX-3(UVC+CDC)
	UART,	/// to FX-3(UVC+CDC) & DBG'd
	USB_SSP	/// to FX-3(SSP)
};

SDKError SDKDRV_Init(Interface interf, const char port_name[], uint32_t baudrate);

SDKError SDKDRV_Open();
SDKError SDKDRV_Close();

SDKError SDKDRV_WriteCameraRegister(uint8_t category, uint16_t address_ofset, uint8_t write_data[], uint8_t write_size);
SDKError SDKDRV_WriteMicomRegister(uint8_t category, uint16_t address_ofset, uint8_t write_data[], uint8_t write_size);

SDKError SDKDRV_ReadCameraRegister(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);
SDKError SDKDRV_ReadMicomRegister(uint8_t  category, uint16_t address_ofset, uint8_t* read_buff_ptr, int read_size);

SDKError SDKDRV_WriteCameraFlash(uint32_t address, uint8_t write_data[], int write_size);
SDKError SDKDRV_WriteMicomFlash(uint32_t address, uint8_t write_data[], int write_size);

SDKError SDKDRV_ReadCameraFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);
SDKError SDKDRV_ReadMicomFlash(uint32_t address, uint8_t* read_buff_ptr, int read_size);

SDKError SDKDRV_UnlockCameraFlash();
SDKError SDKDRV_UnlockMicomFlash();

SDKError SDKDRV_AllWriteCameraFlash();
SDKError SDKDRV_AllWriteMicomFlash();

SDKError SDKDRV_EraseCameraFlash(uint32_t address);
SDKError SDKDRV_EraseMicomFlash(uint32_t address);

SDKError SDKDRV_RebootCamera(void);

SDKError SDKDRV_StartLog(uint8_t mode);
SDKError SDKDRV_StopLog();
SDKError SDKDRV_GetLog(uint8_t index, uint8_t size);

uint8_t  SDKDRV_GetLastError(void);


SDKError SerialOpen(const char port_name[], uint32_t baudrate);
SDKError SerialClose();
SDKError SerialWrite(uint8_t write_data[], int write_size);
SDKError SerialRead(uint8_t* read_buff_ptr, int read_size, uint32_t timeout = 1000);

#ifdef __cplusplus
}
#endif
