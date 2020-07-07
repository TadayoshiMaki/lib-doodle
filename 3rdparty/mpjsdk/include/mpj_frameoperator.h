//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_frameoperator.h
//! \brief Image preview window
//---------------------------------------------------------------------------

#ifndef _mpj_frameoperator_h_
#define _mpj_frameoperator_h_

#include <memory>
#include <future>
#include <atomic>
#include <chrono>
#include "mpj_common.h"
#include "mpj_error.h"
#include "mpj_image.h"
#include "mpj_camera.h"

namespace mpjsdk
{
	//! Threaded camera/image frame callback.
	/*!
	The class provides way to receive callback on every frame from camera or custom frame grabber.

	\code{.cpp}
	// initializes IDS uEye family camera
	auto camera = mpj_Camera::create(mpj_KnownCameras::IDS_uEye);
	auto frameCallback = mpj_ThreadedFrameOperator::create(camera,
		[](auto frame)
		{
			// TODO: process frame (frame is pointer to mpj_Image instance)
		});
	\endcode

	It also equips very easy way to preview the frames on a preview window.

	The following fragment illustrates how to use mpj_ThreadedFrameOperator to preview camera frames.
	\code{.cpp}
	// initializes IDS uEye family camera
	auto camera = mpj_Camera::create(mpj_KnownCameras::IDS_uEye);
	auto frameCallback = mpj_ThreadedFrameOperator::create(camera);

	frameCallback->enablePreview(true, "Camera Preview");
	\endcode
	*/
	class mpj_ThreadedFrameOperator
	{
	public:
		//! Definition of callback called on each frame from camera.
		/*!
		\param frame Frame image from camera. The image is immutable and if you want to modify the pixels, you should firstly call \ref mpj_Image::clone method.
		Please note that the callback should return as soon as possible; otherwise the code may miss incoming frames.
		*/
		using FrameCallback = std::function<void(const mpj_Image* frame)>;

		//! Frame grabber function.
		/*!
		Function to generate frames. The callback is called from a worker thread.
		\return Frame generated.
		*/
		using FrameGrabber = std::function<std::shared_ptr<const mpj_Image>()>;
		//! Finalizer function.
		/*!
		Function to cleanup things.
		*/
		using Finalizer = std::function<void(void)>;

		//! Initialize frame callback with frame grabber.
		/*!
		\param frameGrabber Function to generate frames.
		\param finalizer Function called on destroying the instance.
		\param frameCallback Function called on every frame. You can of course set nullptr to the callback if nothing to do; for the case of simple preview window.
		The callback starts immediately.
		*/
		mpj_ThreadedFrameOperator(const FrameGrabber& frameGrabber, const Finalizer& finalizer = nullptr, const FrameCallback& frameCallback = nullptr) : frameGrabber(frameGrabber), finalizer(finalizer), frameCallback(frameCallback)
		{
			start();
		}

		//! Initializes frame callback.
		/*!
		\param camera Source camera.
		\param frameCallback Function called on every frame. You can of course set nullptr to the callback if nothing to do; for the case of simple preview window.
		The callback starts immediately.
		*/
		mpj_ThreadedFrameOperator(const std::shared_ptr<mpj_Camera>& camera, const FrameCallback& frameCallback = nullptr) : frameCallback(frameCallback)
		{
			camera->startCapture();
			frameGrabber = [camera] { return camera->captureFrameWait(); };
			finalizer = [camera] { camera->stopCapture(); };
			start();
		}

		//! Initialize frame callback with frame grabber.
		/*!
		\param frameGrabber Function to generate frames.
		\param finalizer Function called on destroying the instance.
		\param frameCallback Function called on every frame. You can of course set nullptr to the callback if nothing to do; for the case of simple preview window.
		The callback starts immediately.
		*/
		std::shared_ptr<mpj_ThreadedFrameOperator> create(const FrameGrabber& frameGrabber, const Finalizer& finalizer = nullptr, const FrameCallback& frameCallback = nullptr)
		{
			return std::make_shared<mpj_ThreadedFrameOperator>(frameGrabber, finalizer, frameCallback);
		}

		//! Initializes frame callback.
		/*!
		\param camera Source camera.
		\param frameCallback Function called on every frame. You can of course set nullptr to the callback if nothing to do; for the case of simple preview window.
		The callback starts immediately.
		*/
		std::shared_ptr<mpj_ThreadedFrameOperator> create(const std::shared_ptr<mpj_Camera>& camera, const FrameCallback& frameCallback)
		{
			return std::make_shared<mpj_ThreadedFrameOperator>(camera, frameCallback);
		}

		//! Start frame callback.
		/*!
		If the callback is already started, the function does nothing and return immediately.
		*/
		void start()
		{
			if (!finishing && f.valid())
				return;

			finishing = false;
			f = std::async(std::launch::async, [this]
			{
				try
				{
					while (!finishing)
					{
						auto frame = frameGrabber();
						if (frame)
						{
							if (previewWindow)
								previewWindow->sendImage(frame.get());
							if (frameCallback)
								frameCallback(frame.get());
						}
					}
					finalizer();
					return std::string();
				}
				catch (std::exception& e)
				{
					finalizer();
					return std::string(e.what());
				}
			});
		}

		//! Stop frame callback.
		/*!
		\return error description if any; empty string if no problems.
		If the callback is stopped, the function does nothing and return immediately.
		*/
		std::string stop()
		{
			finishing = true;
			if (f.valid())
				return f.get(); // FIXME: Should we get something?
			return "";
		}

		//! Terminate the camera callback.
		~mpj_ThreadedFrameOperator()
		{
			stop();
		}

		//! Enables camera preview.
		/*!
		\param enable true to enable camera preview window. false to close it.
		\param caption When enabling preview, preview window's caption.

		It internally uses \ref mpj_ImagePreviewWindow to realize the preview window.

		\sa \ref mpj_ImagePreviewWindow
		*/
		void enablePreview(bool enable, const std::string& caption)
		{
			if (enable)
			{
				if (!previewWindow)
					previewWindow = mpj_ImagePreviewWindow::create(caption);
			}
			else
			{
				previewWindow = nullptr;
			}
		}

	private:
		std::atomic_bool finishing;
		FrameGrabber frameGrabber;
		Finalizer finalizer;
		FrameCallback frameCallback;
		std::future<std::string> f;
		std::shared_ptr<mpj_ImagePreviewWindow> previewWindow;

		mpj_ThreadedFrameOperator(const mpj_ThreadedFrameOperator&) = delete;
		mpj_ThreadedFrameOperator& operator=(const mpj_ThreadedFrameOperator&) = delete;
	};
}

#endif // _mpj_frameoperator_h_