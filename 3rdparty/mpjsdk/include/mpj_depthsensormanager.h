//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_depthsensormanager.h
//! \brief Depth sensor related definitions
//---------------------------------------------------------------------------

#ifndef _mpj_depthsensormanager_h_
#define _mpj_depthsensormanager_h_

#include <memory>
#include "mpj_camera.h"

namespace mpjsdk
{
	//! Manager class for depth sensor.
	/*!
	The following fragment starts capturing of depth frames from a depth camera:
	\code{.cpp}
	// Initializes depth camera
	auto dsm = mpj_DepthSensorManager::create();
	auto depthCamera = dsm->createDepthCamera(640, 480, 30);
	depthCamera->startCapture();

	// do our work
	mpj_DepthPoints depthPoints;
	...

	depthPoints.update(depthCamera->captureFrameWait());
	...
	\endcode
	\sa \ref mpj_DepthPoints
	*/
	class mpj_DepthSensorManager : public std::enable_shared_from_this<mpj_DepthSensorManager>
	{
	public:
		//! Initialize an instance.
		static std::shared_ptr<mpj_DepthSensorManager> create();

		//! Get depth camera.
		virtual std::shared_ptr<mpj_Camera> createDepthCamera(uint32_t width, uint32_t height, uint32_t fps) = 0;
		//! Get image camera.
		virtual std::shared_ptr<mpj_Camera> createImageCamera(uint32_t width, uint32_t height, uint32_t fps) = 0;

	protected:
		mpj_DepthSensorManager() {}
		virtual ~mpj_DepthSensorManager() {}
	private:
		mpj_DepthSensorManager(const mpj_DepthSensorManager&) = delete;
		mpj_DepthSensorManager& operator=(const mpj_DepthSensorManager&) = delete;
	};
}

#endif // _mpj_depthsensormanager_h_
