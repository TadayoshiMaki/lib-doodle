//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_camera.h
//! \brief Camera definitions
//---------------------------------------------------------------------------

#ifndef _mpj_camera_h_
#define _mpj_camera_h_

#include <memory>
#include <future>
#include <atomic>
#include <chrono>
#include <map>
#include "mpj_common.h"
#include "mpj_error.h"
#include "mpj_image.h"

namespace mpjsdk
{
#if !defined(DOXYGEN)
	//! Special values to set camera specific value.
	enum class mpj_CameraValue
	{
		Auto = 0, //!< Automatic.
		Minimum = 1, //!< Mimimum value.
		Maximum = 2, //!< Maximum value.
	};

	//! Select what value to get with \ref mpj_Camera parameter get functions.
	enum class mpj_CameraValueQuery
	{
		Current = 0, //!< Current value.
		Minimum = 1, //!< Mimimum value.
		Maximum = 2, //!< Maximum value.
	};
#endif

	//! Camera configuration control.
	/*!
		\code{.cpp}
			auto camera = mpj_Camera::create(mpj_KnownCameras::IDS_uEye);
			camera->startCapture();

			// obtain config manager
			auto configManager = camera->getConfigManager();

			// get minimum/maximum frame rate
			auto minFrameRate = configManager->getFrameRate(mpj_CameraValueQuery::Minimum);
			auto maxFrameRate = configManager->getFrameRate(mpj_CameraValueQuery::Maximum);

			...

			// get current frame rate.
			// Please note that it obtains the real measured frame-rate and
			// if the camera is not initialized yet, it returns 0 or some incorrect value.
			auto currentFrameRate = configManager->getFrameRate();
		\endcode
		\sa \ref mpj_Camera, \ref mpj_Camera::getConfigManager
	*/
	class mpj_CameraConfigManager
	{
	public:
		//! Parameters definitions used mainly by \ref getSupportedConfigs.
		enum mpj_CameraConfigType : uint32_t
		{
			c_FrameRate = 1, //!< Frame rate.
			c_Gain = 2, //!< Gain.
			c_PixelClock = 4, //!< Pixel clock.
			c_AutoGamma = 8, //!< Automatic gamma correction. 
			c_Exposure = 16, //!< Exposure.
			c_AutoWhiteBalance = 32, //!< Automatic white balance.
			c_HardwareGamma = 64, //!< Automatic gamma correction.
		};

		//! Get set of supported parameters.
		/*!
		\return Combination of \ref mpj_CameraConfigType values.
		*/
		virtual uint32_t getSupportedConfigs() const = 0;

		//! Get specific frame rate in frames-per-second.
		/*!
		\param query
		One of \ref mpj_CameraValueQuery enumeration values.<br>
		\ref mpj_CameraValueQuery::Current to get the current frame rate.<br>
		\ref mpj_CameraValueQuery::Maximum to get the maximum frame rate that can be applied.<br>
		\ref mpj_CameraValueQuery::Minimum to get the minimum frame rate that can be applied.<br>
		\return Frames-per-second.
		*/
		virtual float getFrameRate(mpj_CameraValueQuery query = mpj_CameraValueQuery::Current) const = 0;

		//! Set frame rate in frames-per-secound.
		/*!
		\param frameRate Frame rate in frames-per-secound.
		*/
		virtual void setFrameRate(float frameRate) = 0;

		//! Set frame rate with a special frame rate value.
		/*!
		\param frameRate
		One of \ref mpj_CameraValue enumeration values.<br>
		\ref mpj_CameraValue::Auto to make it automatic.<br>
		\ref mpj_CameraValue::Maximum to set the maximum frame rate allowed.<br>
		\ref mpj_CameraValue::Minimum to set the minimum frame rate allowed.<br>
		*/
		virtual void setFrameRate(mpj_CameraValue frameRate) = 0;

		//! Get specific exposure value.
		/*!
		\param query
		One of \ref mpj_CameraValueQuery enumeration values.<br>
		\ref mpj_CameraValueQuery::Current to get the current exposure.<br>
		\ref mpj_CameraValueQuery::Maximum to get the maximum exposure that can be applied.<br>
		\ref mpj_CameraValueQuery::Minimum to get the minimum exposure that can be applied.<br>
		\return Exposure value.
		*/
		virtual float getExposure(mpj_CameraValueQuery query = mpj_CameraValueQuery::Current) const = 0;

		//! Set exposure value.
		/*!
		\param exposure Exposure value.
		*/
		virtual void setExposure(float exposure) = 0;
		/*!
		Set exposure with a special exposure value.
		\param exposure
		One of \ref mpj_CameraValue enumeration values.<br>
		\ref mpj_CameraValue::Auto to make it automatic.<br>
		\ref mpj_CameraValue::Maximum to set the maximum exposure allowed.<br>
		\ref mpj_CameraValue::Minimum to set the minimum exposure allowed.<br>
		*/
		virtual void setExposure(mpj_CameraValue exposure) = 0;

		//! Get specific gain value.
		/*!
		\param query
		One of \ref mpj_CameraValueQuery enumeration values.<br>
		\ref mpj_CameraValueQuery::Current to get the current gain.<br>
		\ref mpj_CameraValueQuery::Maximum to get the maximum gain that can be applied.<br>
		\ref mpj_CameraValueQuery::Minimum to get the minimum gain that can be applied.<br>
		\return Gain value.
		*/
		virtual float getGain(mpj_CameraValueQuery query = mpj_CameraValueQuery::Current) const = 0;

		//! Set gain value.
		/*!
		\param gain Gain value.
		*/
		virtual void setGain(float gain) = 0;

		//! Set gain with a special gain value.
		/*!
		\param gain
		One of \ref mpj_CameraValue enumeration values.<br>
		\ref mpj_CameraValue::Auto to make it automatic.<br>
		\ref mpj_CameraValue::Maximum to set the maximum gain allowed.<br>
		\ref mpj_CameraValue::Minimum to set the minimum gain allowed.<br>
		*/
		virtual void setGain(mpj_CameraValue gain) = 0;

		//! Get specific pixel clock value.
		/*!
		\param query
		One of \ref mpj_CameraValueQuery enumeration values.<br>
		\ref mpj_CameraValueQuery::Current to get the current pixel clock.<br>
		\ref mpj_CameraValueQuery::Maximum to get the maximum pixel clock that can be applied.<br>
		\ref mpj_CameraValueQuery::Minimum to get the minimum pixel clock that can be applied.<br>
		\return Pixel clock value.
		*/
		virtual float getPixelClock(mpj_CameraValueQuery query = mpj_CameraValueQuery::Current) const = 0;

		//! Set pixel clock value.
		/*!
		\param pixelClock Pixel clock value.
		*/
		virtual void setPixelClock(float pixelClock) = 0;

		//! Set pixel clock with a special pixel clock value.
		/*!
		\param pixelClock
		One of \ref mpj_CameraValue enumeration values.<br>
		\ref mpj_CameraValue::Auto to make it automatic.<br>
		\ref mpj_CameraValue::Maximum to set the maximum pixel clock allowed.<br>
		\ref mpj_CameraValue::Minimum to set the minimum pixel clock allowed.<br>
		*/
		virtual void setPixelClock(mpj_CameraValue pixelClock) = 0;

		//! Enable/Disable automatic gamma correction. 
		/*!
		\param enable
		true to enable automatic gamma correction; false to disable it.
		*/
		virtual void enableAutoGamma(bool enable) = 0;

		//! Enable/Disable hardware gamma correction.
		/*!
		\param enable
		true to enable hardware gamma correction; false to disable it.
		*/
		virtual void enableHardwareGamma(bool enable) = 0;

		//! Set gamma correction value.
		/*!
		\param gamma Gamma value.
		*/
		virtual void setGamma(float gamma) = 0;

		//! Get specific gamma correction value.
		/*!
		One of \ref mpj_CameraValueQuery enumeration values.<br>
		\ref mpj_CameraValueQuery::Current to get the current gamma.<br>
		\ref mpj_CameraValueQuery::Maximum to get the maximum gamma that can be applied.<br>
		\ref mpj_CameraValueQuery::Minimum to get the minimum gamma that can be applied.<br>
		\return
		Gamma value.
		*/
		virtual float getGamma(mpj_CameraValueQuery query = mpj_CameraValueQuery::Current) const = 0;

		//! Enable/Disable automatic white balance.
		/*!
		\param enable
			true to enable automatic white balance; false to disable it.
		*/
		virtual void enableAutoWhiteBalance(bool enable) = 0;

		//! Determine whether automatic white balance is enabled or not.
		/*!
		\return true if automatic white balance is enabled.
		*/
		virtual bool isAutoWhiteBalanceEnabled() const = 0;

	protected:
		mpj_CameraConfigManager() {}
		virtual ~mpj_CameraConfigManager() {}
	};

	//! Known camera names.
	class mpj_KnownCameras
	{
	public:
		//! IDS uEye Camera Family
		static constexpr char const IDS_uEye[] = "IDS_uEye";

	private:
		mpj_KnownCameras() = delete;
		mpj_KnownCameras(const mpj_KnownCameras&) = delete;
		mpj_KnownCameras& operator=(const mpj_KnownCameras&) = delete;
	};

	//! Camera class. To create a new instance, use \ref mpj_Camera::create function.
	/*!
	To show camera preview window, see \ref mpj_ThreadedFrameOperator.
	*/
	class mpj_Camera
	{
	public:
		//! Initializes camera.
		/*!
			\param cameraModel Camera model identifier. See \ref mpj_KnownCameras for known camera models.
			\sa \ref mpj_KnownCameras

			The following code illustrates how to use a camera:

			\code{.cpp}
			// initializes IDS uEye family camera
			auto camera = mpj_Camera::create(mpj_KnownCameras::IDS_uEye);

			// setup camera parameters
			auto configManager = camera->getConfigManager();
			configManager->setExposure(mpj_CameraValue::Auto);

			// start capturing frames
			camera->startCapture();

			// capture a frame
			auto image = camera->captureFrame();

			...
			\endcode
			\sa \ref mpj_Camera::create, \ref mpj_CameraConfigManager, \ref mpj_Camera::getConfigManager, \ref mpj_Camera::captureFrame
		*/
		static std::shared_ptr<mpj_Camera> create(const std::string& cameraModel);

		virtual ~mpj_Camera() {}

		/*!
			Get camera model.
		*/
		virtual std::string getCameraModel() const = 0;

		//! Start capture.
		/*!
			It is legal to call \ref startCapture when capture is already started.
		*/
		virtual void startCapture() = 0;

		//! Stop capture.
		/*!
			It is legal to call \ref stopCapture when capture is not started.
			Please note that the destructor anyway call the function implicitly.
		*/
		virtual void stopCapture() = 0;

		//! Determine whether camera is capturing frames or not.
		/*!
			\return true if it is capturing frames; otherwise false.
		*/
		virtual bool isCapturing() const = 0;

		//! Get frame width in pixels.
		virtual uint32_t getWidth() const = 0;
		//! Get frame height in pixels.
		virtual uint32_t getHeight() const = 0;
		//! Get frame pixel format.
		virtual mpj_PixelFormat getFormat() const = 0;

		//!	Capture a latest frame (read-only).
		/*!
			You should call \ref startCapture before calling the function.
			The function only works if \ref canServeReadOnlyFrame returns true.

			\param timeOut Maximum time in milliseconds to wait for a new frame is arrived.
			\return
				If capture succeeded, valid pointer to a \ref mpj_Image instance.<br>
				If timed out, nullptr.<br>
				Otherwise, it throws an exception.
			\sa \ref canServeReadOnlyFrame
		*/
		virtual std::shared_ptr<const mpj_Image> captureFrame(const std::chrono::milliseconds& timeOut = std::chrono::milliseconds(0)) = 0;

		//! Capture a new frame (read-only). The function waits for a new frame.
		/*!
			You should call \ref startCapture before calling the function.<br>
			The function only works if \ref canServeReadOnlyFrame returns true.
		
			\return
				If capture succeeded, valid pointer to a \ref mpj_Image instance.<br>
				If timed out, nullptr.<br>
				Otherwise, it throws an exception.
				\sa \ref canServeReadOnlyFrame
		*/
		virtual std::shared_ptr<const mpj_Image> captureFrameWait() = 0;

		//! Get configuration manager if available.
		/*!
			\ref mpj_CameraConfigManager can be used to get parameters, such as frame-rate (fps), exposure, ... for the camera.
			\return \ref mpj_CameraConfigManager instance if available; otherwise nullptr.
		*/
		virtual const mpj_CameraConfigManager* getConfigManager() const = 0;

		//! Get configuration manager if available.
		/*!
			\ref mpj_CameraConfigManager can be used to set/get parameters, such as frame-rate (fps), exposure, ... for the camera.
			\return \ref mpj_CameraConfigManager instance if available; otherwise nullptr.
		*/
		virtual mpj_CameraConfigManager* getConfigManager() = 0;
	};

	//! Camera registry.
	/*!
	Camera registry is used to resolve camera name to actual camera.
	You can of course, add new motor implementations by modifying the map returned by
	\ref getRegistry.
	*/
	class mpj_CameraRegistry
	{
	public:
		using CameraFactoryFunction = std::function<std::shared_ptr<mpj_Camera>(const std::string&)>;
		using Map = std::map<std::string, CameraFactoryFunction>;

		//! Get camera registry.
		static Map& getRegistry();

		//! Get motor registry entry for name.
		/*!
		\param name Motor name to find.
		\return Pointer to an entry for the name if found; otherwise nullptr.
		*/
		static const CameraFactoryFunction& getEntryFor(const std::string& name)
		{
			static CameraFactoryFunction nullFunc;
			const auto& map = getRegistry();
			auto it = map.find(name);
			if (it != map.end())
				return it->second;
			return nullFunc;
		}

	private:
		mpj_CameraRegistry() = delete;
		mpj_CameraRegistry(const mpj_CameraRegistry&) = delete;
		mpj_CameraRegistry& operator=(const mpj_CameraRegistry&) = delete;
	};
}

#endif // _mpj_camera_h_