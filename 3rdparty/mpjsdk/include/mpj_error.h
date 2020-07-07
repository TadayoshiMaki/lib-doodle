//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_error.h
//! \brief Error/Exception related definitions
//---------------------------------------------------------------------------

#ifndef _mpj_error_h_
#define _mpj_error_h_

#include <stdexcept>
#include <filesystem>
#include <string>
#include "mpj_common.h"
#include "mpj_string.h"
#include "mpj_debug.h"

namespace mpjsdk
{
#if !defined(DOXYGEN)
	//! Error code definitions.
	enum class mpj_Error : int32_t
	{
		Success = 0, //!< Success.
		InvalidParam, //!< Invalid parameter.
		NotImplemented, //!< The function called is not implemented.
		NotSupported, //!< Not supported.
		NotFound, //!< Not found.
		IoError, //!< I/O error.
		FunctionFailure, //!< Function failure.
		InvalidStatus, //!< Invalid status.
		DataBroken, //!< Data broken.
		WrongFallThrough, //!< Logic error.
		UnknownError = -1 //!< Unknown error.
	};
#endif

	//! Abstract exception definition with explicit error code.
	class mpj_Exception : public std::runtime_error
	{
	public:
		//! Initialize mpj_Exception with a message.
		/*! \param message Error message. */
        mpj_Exception(const char* message) : std::runtime_error(message) { ; }

		//! Initialize mpj_Exception with a message.
		/*! \param message Error message. */
        mpj_Exception(const std::string& message) : std::runtime_error(message) { ; }

		//! Get error code for the exception.
		/*! \return Error code. */
		virtual mpj_Error getError() const noexcept = 0;

		//! Get error code from exception.
		/*!
			\param e Exception to extract error code from.
			\return
				If the exception is \ref mpj_Exception, the function returns the error code
				from the exception; otherwise it returns \ref mpj_Error::UnknownError.
		*/
		static mpj_Error errorFromException(const std::exception& e) noexcept
		{
			auto me = dynamic_cast<const mpj_Exception*>(&e);
			return me ? me->getError() : mpj_Error::UnknownError;
		}
	};

	//! A template class to easily define \ref mpj_Exception based on \ref mpj_Error.
	template<mpj_Error ErrorCode> class mpj_ExceptionBase : public mpj_Exception
	{
	public:
		//! Initialize mpj_Exception with a message.
		/*! \param message Error message. */
        explicit mpj_ExceptionBase(const char* message) : mpj_Exception(message) { ; }

		//! Initialize mpj_Exception with a message. (For debug builds)
		/*!
			\param line Line number (__LINE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param file File name (__FILE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param message Error message.
		*/
		mpj_ExceptionBase(int line, const char* file, const char* message) : mpj_ExceptionBase(mpj_StringUtils::format("%s(%d): error %d: %s", file, line, ErrorCode, message)) {}

		//! Initialize mpj_Exception with a message.
		/*!
			\param message Error message.
		*/
		explicit mpj_ExceptionBase(const std::string& message) : mpj_ExceptionBase(message.c_str()) {}

		//! Initialize mpj_Exception with a message. (For debug builds)
		/*!
			\param line Line number (__LINE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param file File name (__FILE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param message Error message.
		*/
		mpj_ExceptionBase(int line, const char* file, const std::string& message) : mpj_ExceptionBase(line, file, message.c_str()) {}

		//! Initialize mpj_Exception with a message.
		/*!
			\param message Error message.
		*/
		template<typename ... Args> mpj_ExceptionBase(const std::string& format, Args ... args) : mpj_ExceptionBase(mpj_StringUtils::format(format, args ...)) {}

		//! Initialize mpj_Exception with a message. (For debug builds)
		/*!
			\param line Line number (__LINE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param file File name (__FILE__) in the source code; internally called from \ref nullCheck and \ref throwOnFail macros.
			\param format Error message format.
		*/
		template<typename ... Args> mpj_ExceptionBase(int line, const char* file, const std::string& format, Args ... args) : mpj_ExceptionBase(line, file, mpj_StringUtils::format(format, args ...)) {}
		mpj_Error getError() const noexcept override { return ErrorCode; }
	};

	//! For \ref mpj_Error::InvalidParam.
	typedef mpj_ExceptionBase<mpj_Error::InvalidParam> mpj_InvalidParamException;
	//! For \ref mpj_Error::NotImplemented.
	typedef mpj_ExceptionBase<mpj_Error::NotImplemented> mpj_NotImplementedException;
	//! For \ref mpj_Error::NotSupported.
	typedef mpj_ExceptionBase<mpj_Error::NotSupported> mpj_NotSupportedException;
	//! For \ref mpj_Error::NotSupported.
	typedef mpj_ExceptionBase<mpj_Error::NotFound> mpj_NotFoundException;
	//! For \ref mpj_Error::IoError.
	typedef mpj_ExceptionBase<mpj_Error::IoError> mpj_IoException;
	//! For \ref mpj_Error::FunctionFailure.
	typedef mpj_ExceptionBase<mpj_Error::FunctionFailure> mpj_FunctionFailureException;
	//! For \ref mpj_Error::DataBroken.
	typedef mpj_ExceptionBase<mpj_Error::DataBroken> mpj_DataBrokenException;
	//! For \ref mpj_Error::InvalidStatus.
	typedef mpj_ExceptionBase<mpj_Error::InvalidStatus> mpj_InvalidStatusException;
	//! For \ref mpj_Error::DataBroken.
	typedef mpj_ExceptionBase<mpj_Error::DataBroken> mpj_DataBrokenException;
	//! For \ref mpj_Error::WrongFallThrough.
	typedef mpj_ExceptionBase<mpj_Error::WrongFallThrough> mpj_WrongFallThroughException;
	//! For \ref mpj_Error::UnknownError.
	typedef mpj_ExceptionBase<mpj_Error::UnknownError> mpj_UnknownException;

	//! An exception dedicated to notify the filename.
	class mpj_FileIoException : public mpj_Exception
	{
	public:
		//! Initialize mpj_FileIoException based on a file name and an exception already thrown.
		/*!
		On Windows, the file name in \ref std::exception::what() is locale specific encoding and
		may be broken if it contains certain Unicode characters.
		\param fileName Target file name.
		\param error Error code.
		\param message Error message.
		*/
		mpj_FileIoException(const _FSPFX path& fileName, mpj_Error error, const char* message) :
			mpj_Exception(mpj_StringUtils::format("%s: %s", message, fileName.generic_string().c_str())),
			error(error), fileName(fileName)
		{
		}

		//! Initialize mpj_FileIoException based on a file name and an exception already thrown.
		/*!
		On Windows, the file name in \ref std::exception::what() is locale specific encoding and
		may be broken if it contains certain Unicode characters.
		\param fileName Target file name.
		\param error Error code.
			\param message Error message.
		*/
		mpj_FileIoException(int line, const char* file, const _FSPFX path& fileName, mpj_Error error, const char* message) :
			mpj_Exception(mpj_StringUtils::format("%s(%d): %s: %s", file, line, message, fileName.generic_string().c_str())),
			error(error), fileName(fileName)
		{
		}

		//! Initialize mpj_FileIoException based on a file name and an exception already thrown.
		/*!
		On Windows, the file name in \ref std::exception::what() is locale specific encoding and
		may be broken if it contains certain Unicode characters.
		\param fileName Target file name.
		\param e The exception thrown and caught.
		*/
		mpj_FileIoException(const _FSPFX path& fileName, const std::exception& e) :
			mpj_Exception(mpj_StringUtils::format("%s: %s", e.what(), fileName.generic_string().c_str())),
			error(errorFromException(e)), fileName(fileName)
		{
		}

		//! Initialize mpj_FileIoException based on a file name and an exception already thrown.
		/*!
		On Windows, the file name in \ref std::exception::what() is locale specific encoding and
		may be broken if it contains certain Unicode characters.
		\param fileName Target file name.
		\param e The exception thrown and caught.
		*/
		mpj_FileIoException(int line, const char* file, const _FSPFX path& fileName, const std::exception& e) :
			mpj_Exception(mpj_StringUtils::format("%s(%d): %s: %s", file, line, e.what(), fileName.generic_string().c_str())),
			error(errorFromException(e)), fileName(fileName)
		{
		}

		//! Get error code for the exception.
		/*! \return Error code. */
		mpj_Error getError() const noexcept override { return error; }

		//! Get the target file name.
		/*! \return File name. */
		const _FSPFX path& getFileName() const noexcept { return fileName; }

	private:
		mpj_Error error;
		_FSPFX path fileName;
	};

	//! Called internally from \ref nullCheck macro.
	template<typename T> T* nullCheck_(T* ptr, const char* name, int line = 0, const char* file = nullptr)
	{
		if (!ptr)
			throw mpj_InvalidParamException(line, file, mpj_StringUtils::format("%s is nullptr.", name));
		return ptr;
	}

	//! Called internally from \ref throwOnFail macro.
	template<typename T> void throwOnFail_(T f, const char* name, int line = 0, const char* file = nullptr)
	{
		if (!f)
			throw mpj_FunctionFailureException(line, file, name);
	}

	//! Called internally from \ref throwOnFail macro.
	template<typename T> void throwOnFail_(T f, const std::string& name, int line = 0, const char* file = nullptr)
	{
		if (!f)
			throw mpj_FunctionFailureException(line, file, name);
	}
	//! Reserved for internal use only.
	template<typename TT, typename T> TT* nullCheckAndCast(T* ptr, const char* name, int line = 0, const char* file = nullptr)
	{
		return reinterpret_cast<TT*>(nullCheck_(ptr, name, line, file));
	}

#if defined(_DEBUG)
//! Throw if the passed value is nullptr.
#define nullCheck(v) mpjsdk::nullCheck_(v,#v " is nullptr.",__LINE__,__FILE__)
//! Throw if the value evaluated is not true.
#define throwOnFail(f,name) mpjsdk::throwOnFail_(f,name,__LINE__,__FILE__)
//! Throw if the value evaluated is not true.
#define throwOnFailNoName(f) mpjsdk::throwOnFail_(f,#f,__LINE__,__FILE__)
//! Insert line and file name on debug.
#define _LINE_AND_FILE __LINE__,__FILE__,
#else
//! Throw if the passed value is nullptr.
#define nullCheck(v) mpjsdk::nullCheck_(v,#v)
//! Throw if the value evaluated is not true.
#define throwOnFail(v,name) mpjsdk::throwOnFail_(v,name)
//! Throw if the value evaluated is not true.
#define throwOnFailNoName(f) mpjsdk::throwOnFail_(f,#f)
//! Insert line and file name on debug.
#define _LINE_AND_FILE
#endif
}

#endif // _mpj_error_h_

