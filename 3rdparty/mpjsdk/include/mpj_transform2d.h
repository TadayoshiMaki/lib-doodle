//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_transform2d.h
//! \brief 2D Point-to-point transform
//---------------------------------------------------------------------------

#ifndef _mpj_pointtransform_h_
#define _mpj_pointtransform_h_

#include <memory>
#include <filesystem>
#include "mpj_common.h"
#include "mpj_error.h"

namespace mpjsdk
{
	//! Generic 2D point-to-point transform.
	class mpj_Transform2d
	{
	public:
		//! Load transformation table from a (int,int) to (float,float) CSV file.
		/*!
			The function accepts CSV (Comma-separated-values) file, on which each line contains 4 values of fromX (int),fromY (int),toX (float),toY (float) like the following fragment:
			\code{.csv}
			0,0,23.2,5.1
			1,0,24.2,5.12
			...
			\endcode
			\param filePath File path for transformation table.
			\return new \ref LensDistortionCorrection instance.
		*/
		static std::shared_ptr<mpj_Transform2d> createFromIIFFCsvTable(const _FSPFX path & tableCsvFileName);

		//! Transform point from original coordinate system to nomalized coordinate system.
		/*!
			\param fromX X-coordinate of the original point.
			\param fromY Y-coordinate of the original point.
			\param toX X-coordinate of the transformed point.
			\param toY Y-coordinate of the transformed point.
			\return
				true if the transformation succeeded.
		*/
		virtual bool transform(float fromX, float fromY, float& toX, float& toY) const = 0;

	protected:
		mpj_Transform2d() {}
		virtual ~mpj_Transform2d() {}
	private:
		mpj_Transform2d(const mpj_Transform2d&) = delete;
		mpj_Transform2d& operator=(const mpj_Transform2d&) = delete;
	};
}

#endif // _mpj_pointtransform_h_
