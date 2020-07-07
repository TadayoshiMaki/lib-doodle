//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_depth.h
//! \brief Depth related definitions
//---------------------------------------------------------------------------

#ifndef _mpj_depth_h_
#define _mpj_depth_h_

#include <memory>
#include "mpj_image.h"
#include "mpj_vector3d.h"
#include "mpj_matrix4d.h"

namespace mpjsdk
{
	//! Manages depth points.
	/*!
	The following fragment starts capturing of depth frames from a depth camera:
	\code{.cpp}
		// Initializes depth camera
		auto dsm = mpj_DepthSensorManager::create();
		auto depthCamera = dsm->createDepthCamera(640, 480, 30);
		depthCamera->startCapture();

		// do our work
		mpj_DepthPoints depthPoints(depthCamera->captureFrameWait(), rotation, sensorOffsetVector);
		...
	\endcode
	\sa \ref mpj_DepthSensorManager
	*/
	class mpj_DepthPoints
	{
	public:
		//! Initialize an instance with a depth frame.
		/*!
		\param depthFrame
		Depth frame. The image format should be mpj_PixelFormat::Gray16.
		Typically, the depth frames are from a depth camera managed by \ref mpj_DepthSensorManager.
		\param matrix Matrix to apply to all the points.
		\param translationIfNeeded Additional translation to the matrix, which is typically the offset to the sensor from the rotation origin.
		*/
		explicit mpj_DepthPoints(const mpj_Image* depthFrame, const mpj_Mat4& matrix = mpj_Mat4(), const mpj_Vec3& translationIfNeeded = mpj_Vec3())
		{
			if (!depthFrame)
				throw mpj_InvalidParamException(_LINE_AND_FILE "mpj_DepthPoints::update: depthFrame should not be nullptr.");
			if (depthFrame->getFormat() != mpj_PixelFormat::Gray16)
				throw mpj_InvalidParamException(_LINE_AND_FILE "mpj_DepthPoints::update: depthFrame should be in Gray16 format.");

			auto w = depthFrame->getWidth();
			auto h = depthFrame->getHeight();
			points.resize(w * h);
			width = w;
			height = h;
			matrixApplied = matrix;

			auto p3d = &points[0];
			for (uint32_t y = 0; y < h; y++)
			{
				auto pDepth = depthFrame->getPixelPtr<uint16_t>(0, y);
				for (uint32_t x = 0; x < w; x++)
					*p3d++ = depthToWorld(mpj_Vec3(static_cast<float>(x), static_cast<float>(y), *pDepth++));
			}

			auto mat = matrix;
			mat(0, 3) = translationIfNeeded.x;
			mat(1, 3) = translationIfNeeded.y;
			mat(2, 3) = translationIfNeeded.z;

			if (!mat.isIdentity())
			{
				for (auto& p : points)
					p = matrix * p;
			}
		}

		static constexpr float INV_FX = 1.f / 526.37013657f;
		static constexpr float INV_FY = 1.f / 526.37013657f;
		static constexpr float CX = 313.68782938f;
		static constexpr float CY = 259.01834898f;

		//! Depth to world coordinate conversion.
		static constexpr mpj_Vec3 depthToWorld(const mpj_Vec3& v)
		{
			return mpj_Vec3(
				-(v.x - CX) * v.z * INV_FX,
				-(v.y - CY) * v.z * INV_FY,
				v.z);
		}

		//! World to depth coordinate conversion.
		static constexpr mpj_Vec3 worldToDepth(const mpj_Vec3& v)
		{
			return mpj_Vec3(
				-v.x / (v.z * INV_FX) + CX,
				-v.y / (v.z * INV_FY) + CY,
				v.z);
		}

		//! Get result points.
		const std::vector<mpj_Vec3>& getPoints() const { return points; }
		//! Get the original frame width in pixels.
		uint32_t getWidth() const { return width; }
		//! Get the original frame height in pixels.
		uint32_t getHeight() const { return height; }
		//! Get the matrix applied to the pixels (the matrix passed to the constructor).
		const mpj_Mat4& getMatrixApplied() const { return matrixApplied; }
		
	private:
		uint32_t width, height;
		mpj_Mat4 matrixApplied;
		std::vector<mpj_Vec3> points;
	};
}

#endif // _mpj_depth_h_

