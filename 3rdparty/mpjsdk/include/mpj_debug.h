//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_debug.h
//! \brief Definitions for debugging
//---------------------------------------------------------------------------

#ifndef _mpj_debug_h_
#define _mpj_debug_h_

#include <string>

namespace mpjsdk
{
	//! Helper functions for debugging purpose.
	class mpj_Debug
	{
	public:
		//! Output log message.
		/*! \param message message. */
		static void log(const char* message);

		//! Output log message.
		/*! \param message message. */
		static void log(const std::string& message) { log(message.c_str()); }

		//! Log output function prototype.
		/*!
		\param context Context passed to \ref setLogOutputFunction.
		\param message message.
		*/
		typedef void (*LogOutputFunction)(void* context, const char* message);

		//! Override default log putput function.
		/*!
		\param context Context passed to the log output function.
		\param logOutput Log output function.
		*/
		static void setLogOutputFunction(void* context, LogOutputFunction logOutput);

	private:
		mpj_Debug() = delete;
		mpj_Debug(const mpj_Debug&) = delete;
		mpj_Debug& operator=(const mpj_Debug&) = delete;
	};
}

#endif // _mpj_debug_h_
