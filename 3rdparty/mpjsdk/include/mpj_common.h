//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_common.h
//! \brief Common definitions
//---------------------------------------------------------------------------

#ifndef _mpj_common_h_
#define _mpj_common_h_

#include <algorithm>
#include <string>

namespace mpjsdk
{
	//! Information about the library.
	class mpj_Library
	{
	public:
		//! Get human-readable version of the library.
		/*!
			\return
				Returns version string of the following format: X.Y.REV (COMMIT) BITNESS RELEASE|DEBUG
		*/
		static const char* getVersion();

		//! Get revision of the library.
		/*!
			\return
				The revision of the library. The internal build may have revision of 0.
		*/
		static uint32_t getRevision();

		//! Determine whether the library is debug version or not.
		/*!
			\return
				true if the library is debug version; otherwise false.
		*/
		static bool isDebugVersion();

	private:
		mpj_Library() = delete;
		mpj_Library(const mpj_Library&) = delete;
		mpj_Library& operator=(const mpj_Library&) = delete;
	};
}

#endif // _mpj_common_h_
