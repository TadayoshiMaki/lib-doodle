//------------------------------------------------------------------------------
//  Sentech StApi
//  Copyright (c) 2017 OMRON SENTECH CO., LTD
//  http://www.sentech.co.jp
//------------------------------------------------------------------------------
/*!
	@ingroup	StApiTL_PublicImpl
	@file
	@~english	@details	Include file for using StApi Transport Layer module.
	@~japanese	@details	StApi トランスポート層モジュール用インクルードファイル
*/
#ifndef STAPI_TL_H__
#define STAPI_TL_H__

#ifdef _WIN32
#   define _WINSOCKAPI_	// To prevent including winsock.h when included windows.h which has WinSock2.h.
#endif

#if !defined(STAPI_DEBUG_BUILD)
#	if !defined(GENICAM_USER_ALWAYS_LINK_RELEASE)
#		define GENICAM_USER_ALWAYS_LINK_RELEASE
#	endif
#endif

#include "StApi_Linkage.h"
#include "StApi_Utilities.h"


#include "CStGenApiCallbackEx.h"
#include "CStCallback.h"
#include "CStException.h"

#include "StApi_TLFunctions.h"


#if defined(_WIN32) && !defined(STSDK_TL_EXPORTS)
#pragma comment(lib, ST_FILE_NAME("STApi_TL", ".lib"))
#endif

#endif //STAPI_TL_H__
