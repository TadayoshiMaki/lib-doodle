//==============================================================================
// Copyright (C) <2014> Sony Corp. All Rights Reserved.
//
// This program is proprietary and confidential. By using this program
// you agree to the terms of the associated Software License Agreement.
//------------------------------------------------------------------------------
//! @file
//! @brief  Definitions of standard types.
//==============================================================================

#pragma once

#if defined(WIN32)
#include <stddef.h>
#include <stdint.h>
#elif defined(__WINSYS)
#include <stddef.h>
#else
#include <stddef.h>
#if !defined(__cplusplus)
#include <stdbool.h>
#endif // !defined(__cplusplus)
#include <stdint.h>
#endif // defined(WIN32)

//------------------------------------------------------------------------------
// define and structures
//------------------------------------------------------------------------------
#if !defined(__cplusplus)
#define inline _inline //!< "inline" keyword
#endif // !defined(__cplusplus)

#if (defined(WIN32) || defined(__WINSYS)) && !defined(__cplusplus)
typedef unsigned char bool; //!< Boolean type
#define false (0 == 1)      //!< "false" value
#define true  (1 == 1)      //!< "true" value
#endif // (defined(WIN32) || defined(__WINSYS)) && !defined(__cplusplus)

#if defined(__WINSYS)
//! unsigned bit width type
typedef unsigned char      uint8_t;  //!< unsigned 8-bit type
typedef unsigned short     uint16_t; //!< unsigned 16-bit type
typedef unsigned int       uint32_t; //!< unsigned 32-bit type
typedef unsigned long long uint64_t; //!< unsigned 64-bit type

//! maximum value of each unsigned bit width type
#define UINT8_MAX  UCHAR_MAX      //!< maximum value of unsigned 8-bit type
#define UINT16_MAX USHRT_MAX      //!< maximum value of unsigned 16-bit type
#define UINT32_MAX UINT_MAX       //!< maximum value of unsigned 32-bit type
//#define UINT64_MAX ULLONG_MAX //!< maximum value of unsigned 64-bit type
#define UINT64_MAX ULONG_LONG_MAX //!< maximum value of unsigned 64-bit type

//! signed bit width type
typedef signed char      int8_t;  //!< signed 8-bit type
typedef signed short     int16_t; //!< signed 16-bit type
typedef signed int       int32_t; //!< signed 32-bit type
typedef signed long long int64_t; //!< signed 64-bit type

//! minimum and maximum value of each signed bit width type
#define INT8_MIN  SCHAR_MIN     //!< minimum value of signed 8-bit type
#define INT8_MAX  SCHAR_MAX     //!< maximum value of signed 8-bit type
#define INT16_MIN SHRT_MIN      //!< minimum value of signed 16-bit type
#define INT16_MAX SHRT_MAX      //!< maximum value of signed 16-bit type
#define INT32_MIN INT_MIN       //!< minimum value of signed 32-bit type
#define INT32_MAX INT_MAX       //!< maximum value of signed 32-bit type
//#define INT64_MIN LLONG_MIN //!< minimum value of signed 64-bit type
#define INT64_MIN LONG_LONG_MIN //!< minimum value of signed 64-bit type
//#define INT64_MAX LLONG_MAX //!< maximum value of signed 64-bit type
#define INT64_MAX LONG_LONG_MAX //!< maximum value of signed 64-bit type
#endif // defined(__WINSYS)

#if defined(WIN32) || defined(__WINSYS)
//! byte order define
#if !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN)
#define __LITTLE_ENDIAN //!< enable little endian
#endif // !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN)

//! byte order functions
#if defined(__LITTLE_ENDIAN)
//! convert 16-bit CPU endian value to little endian
#define __cpu_to_le16(x) ((uint16_t) (x))
//! convert 32-bit CPU endian value to little endian
#define __cpu_to_le32(x) ((uint32_t) (x))
//! convert 64-bit CPU endian value to little endian
#define __cpu_to_le64(x) ((uint64_t) (x))
//! convert 16-bit CPU endian value to big endian
#define __cpu_to_be16(x) (((((uint16_t) (x)) & 0xFF) << 8) | ((((uint16_t) (x)) & 0xFF00) >> 8))
//! convert 32-bit CPU endian value to big endian
#define __cpu_to_be32(x) (((((uint32_t) (x)) & 0xFF)     << 24) | ((((uint32_t) (x)) & 0xFF00)     <<  8) | \
                          ((((uint32_t) (x)) & 0xFF0000) >>  8) | ((((uint32_t) (x)) & 0xFF000000) >> 24))
//! convert 64-bit CPU endian value to big endian
#define __cpu_to_be64(x) ((((uint64_t)(x) & 0x00000000000000FF) << 56) | \
                          (((uint64_t)(x) & 0x000000000000FF00) << 40) | \
                          (((uint64_t)(x) & 0x0000000000FF0000) << 24) | \
                          (((uint64_t)(x) & 0x00000000FF000000) <<  8) | \
                          (((uint64_t)(x) & 0x000000FF00000000) >>  8) | \
                          (((uint64_t)(x) & 0x0000FF0000000000) >> 24) | \
                          (((uint64_t)(x) & 0x00FF000000000000) >> 40) | \
                          (((uint64_t)(x) & 0xFF00000000000000) >> 56) )
#elif defined(__BIG_ENDIAN)
//! convert 16-bit CPU endian value to little endian
#define __cpu_to_le16(x) (((((uint16_t) (x)) & 0xFF) << 8) | ((((uint16_t) (x)) & 0xFF00) >> 8))
//! convert 32-bit CPU endian value to little endian
#define __cpu_to_le32(x) (((((uint32_t) (x)) & 0xFF)     << 24) | ((((uint32_t) (x)) & 0xFF00)     <<  8) | \
                          ((((uint32_t) (x)) & 0xFF0000) >>  8) | ((((uint32_t) (x)) & 0xFF000000) >> 24))
//! convert 64-bit CPU endian value to little endian
#define __cpu_to_le64(x) ((((uint64_t)(x) & 0x00000000000000FF) << 56) | \
                          (((uint64_t)(x) & 0x000000000000FF00) << 40) | \
                          (((uint64_t)(x) & 0x0000000000FF0000) << 24) | \
                          (((uint64_t)(x) & 0x00000000FF000000) <<  8) | \
                          (((uint64_t)(x) & 0x000000FF00000000) >>  8) | \
                          (((uint64_t)(x) & 0x0000FF0000000000) >> 24) | \
                          (((uint64_t)(x) & 0x00FF000000000000) >> 40) | \
                          (((uint64_t)(x) & 0xFF00000000000000) >> 56) )
//! convert 16-bit CPU endian value to big endian
#define __cpu_to_be16(x) ((uint16_t) (x))
//! convert 32-bit CPU endian value to big endian
#define __cpu_to_be32(x) ((uint32_t) (x))
//! convert 64-bit CPU endian value to big endian
#define __cpu_to_be64(x) ((uint64_t) (x))
#else
#error unknown endian! define "__LITTLE_ENDIAN" or "__BIG_ENDIAN"
#endif // defined(__LITTLE_ENDIAN)

#define __le16_to_cpu(x) __cpu_to_le16(x) //!< convert 16-bit little endian value to CPU endian
#define __le32_to_cpu(x) __cpu_to_le32(x) //!< convert 32-bit little endian value to CPU endian
#define __le64_to_cpu(x) __cpu_to_le64(x) //!< convert 64-bit little endian value to CPU endian
#define __be16_to_cpu(x) __cpu_to_be16(x) //!< convert 16-bit big endian value to CPU endian
#define __be32_to_cpu(x) __cpu_to_be32(x) //!< convert 32-bit big endian value to CPU endian
#define __be64_to_cpu(x) __cpu_to_be64(x) //!< convert 64-bit big endian value to CPU endian
#endif // defined(WIN32) || defined(__WINSYS)


//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------


