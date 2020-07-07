//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_geodesicdomemapper.h
//! \brief Geodesic dome mapping algorithm
//---------------------------------------------------------------------------
#ifndef _mpj_geodesicdomemapper_h_
#define _mpj_geodesicdomemapper_h_

#include "mpj_geodesicdome.h"
#include "mpj_common.h"
#include "mpj_error.h"
#include "mpj_angle.h"
#include "mpj_vector3d.h"
#include "mpj_depth.h"
#include "mpj_motor.h"
#include "mpj_camera.h"
#include <cmath>
#include <queue>
#include <future>

namespace mpjsdk
{
	//! Helper class to genrate geodesic dome from motor rotated depth camera.
	/*!
	The following fragment illustrates how to create geodesic dome by scanning around the device:
	\code{.cpp}
	// projector width/height in pixels
	uint32_t projectorWidth = 1920;
	uint32_t projectorHeight = 1080;
	auto depthCameraTranslate = mpj_Angle(...); // offset from motor rotation center to depth-camera

	// There are many magic numbers/strings here; please update them according to your environment.
	auto motorManager = mpj_MotorManager::create(mpj_KnownMotors::DXL_MX28T, "COM8");
	auto depthCamera = mpj_DepthSensorManager::create()->createDepthCamera(640, 480, 30);
	auto depthCamera->startCapture();

	// create geodesic dome by scanning around by the depth-camera
	mpj_GeodesicDomeMapper geodesicDomeMapper(depthCameraTranslate);

	// run mapping
	// NOTE: it takes long and you'd better call it on background thread
	auto geodesicDome = geodesicDomeMapper.doMappingAsync(depthCamera, motorManager);

	...
	\endcode
	\sa \ref mpj_GeodesicDome
	*/
	class mpj_GeodesicDomeMapper
	{
	public:
		//! Create geodesic dome mapper.
		/*!
		The function blocks until all the mapping process finishes.
		\param cameraTranslation Depth camera offset from the rotation origin.
		\param level
		*/
		mpj_GeodesicDomeMapper(const mpj_Vec3& cameraTranslation, int level = mpj_GeodesicDome::DefaultGeodesicDomeLevel) :
			cameraTranslation(cameraTranslation), finalized(false)
		{
			geodesicDome = std::make_shared<mpj_GeodesicDome>(level);
		}

		//! Do a mapping at the specified angle with the specified depth frame.
		/*!
		\param depthFrame Depth frame from the camera.
		\param angle Motor/camera angle.
		*/
		void doMappingAtAngle(const mpj_Image* depthFrame, const mpj_Angle& angle)
		{
			throwIfFinalized();
			auto rotation = mpj_Mat4::createFromAngle(angle);
			geodesicDome->updateCloud(mpj_DepthPoints(depthFrame, rotation, cameraTranslation), rotation.inverse());
		}

		//! Finalize the mapping and return \ref mpj_GeodesicDome instance.
		/*!
			If the mapping is already finalized, the function simply returns the last result.
			\return The geodesic dome.
		*/
		std::shared_ptr<mpj_GeodesicDome> finalize()
		{
			if (!finalized)
			{
				geodesicDome->smoothCloud();
				finalized = true;
			}
			return geodesicDome;
		}

		//! Mapping by rotating the camera.
		/*!
			The function blocks until all the mapping process finishes.
			\param depthCamera Depth camera.
			\param motorManager Motor manager to control the motor.
		*/
		std::shared_ptr<mpj_GeodesicDome> doMapping(const std::shared_ptr<mpj_Camera>& depthCamera, const std::shared_ptr<mpj_MotorManager>& motorManager)
		{
			throwIfFinalized();
			saveMotorParams = motorManager->getMotorParamsLastApplied();
			auto params = saveMotorParams;
			params.params[0].maxRpm = 10.f;
			params.params[0].pGain = 64;
			params.params[1].maxRpm = 10.f;
			params.params[1].pGain = 64;
			motorManager->applyMotorParams(params);

			static constexpr std::array<float, 8> PAN_STOPS{ -75, -60, -40, -20, 20, 40, 60, 75 };
			static constexpr std::array<float, 4> TILT_STOPS{ -40, -5, 30, 65 };

			auto f = std::async(std::launch::async, [this] { backgroundQueueConsumer(); });

			for (int i = 0; i < PAN_STOPS.size(); i++)
			{
				auto pan = PAN_STOPS[i];
				for (int j = 0; j < TILT_STOPS.size(); j++)
				{
					auto tilt = (i & 1) ? TILT_STOPS[TILT_STOPS.size() - j - 1] : TILT_STOPS[j];
					auto goalAngle = mpj_Angle(pan, tilt);

					moveMotorWait(motorManager, goalAngle);

					auto angle = motorManager->getCurrentAngle();
					auto frame = depthCamera->captureFrameWait()->clone();

					auto rotation = mpj_Mat4::createFromAngle(angle);
					post(std::make_shared<mpj_DepthPoints>(frame.get(), rotation, cameraTranslation));
				}
			}
			moveMotorWait(motorManager, mpj_Angle::zero());

			// terminate the queue
			post(nullptr);
			f.wait();

			motorManager->applyMotorParams(saveMotorParams);
			return finalize();
		}

	private:
		mpj_Vec3 cameraTranslation;
		mpj_MotorParams saveMotorParams;
		std::shared_ptr<mpj_GeodesicDome> geodesicDome;

		std::condition_variable cvQueue;
		std::mutex mutexQueue;
		std::queue<std::shared_ptr<mpj_DepthPoints>> queue;

		bool finalized;

		void moveMotorWait(const std::shared_ptr<mpj_MotorManager>& motorManager, const mpj_Angle& goalAngle)
		{
			motorManager->setGoalAngle(goalAngle);
			waitForMotorStop(motorManager);
		}

		void waitForMotorStop(const std::shared_ptr<mpj_MotorManager>& motorManager)
		{
			while (motorManager->isMotorRotating()); // FIXME: busy loop
		}

		void throwIfFinalized()
		{
			if (finalized)
				throw mpj_InvalidStatusException(_LINE_AND_FILE "mpj_GeodesicDomeMapper: already finalized.");
		}

		void post(const std::shared_ptr<mpj_DepthPoints>& req)
		{
			{
				std::unique_lock<std::mutex> lock(mutexQueue);
				queue.push(req);
			}
			cvQueue.notify_all();
		}

		void backgroundQueueConsumer()
		{
			while (true)
			{
				std::shared_ptr<mpj_DepthPoints> req;
				{
					std::unique_lock<std::mutex> lock(mutexQueue);
					cvQueue.wait(lock, [this] { return !queue.empty(); });
					req = queue.front();
					queue.pop();
				}

				if (!req) // nullptr to terminate the thread
					break;

				geodesicDome->updateCloud(*req, req->getMatrixApplied().inverse());
			}
		}
	};
}

#endif // _mpj_geodesicdomemapper_h_

