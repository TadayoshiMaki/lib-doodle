//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_pointdetector.h
//! \brief Point detector definitions
//---------------------------------------------------------------------------

#ifndef _mpj_pointerdetector_h_
#define _mpj_pointerdetector_h_

#include <memory>
#include "mpj_common.h"
#include "mpj_angle.h"
#include "mpj_transform2d.h"
#include "mpj_image.h"

namespace mpjsdk
{
	//! Point detected by \ref mpj_PointerDetector
	struct mpj_DetectedPoint
	{
		float			x;				//!< x coordinate
		float			y;				//!< y coordinate
		float			size;			//!< Point size
		float			aveBrightness;	//!< average pixel value of pointing area

		//! Initialize an instance.
		mpj_DetectedPoint() :
			x(0.f),
			y(0.f),
			size(0.f),
			aveBrightness(0.f)
		{
		}

		//! Initialize an instance.
		mpj_DetectedPoint(float x, float y, float size, float aveBrightness) :
			x(x),
			y(y),
			size(size),
			aveBrightness(aveBrightness)
		{
		}
	};

	//! Detect points on frames.
	/*!
		\code{.cpp}
			auto detector = mpj_PointDetector::create();

			// set mask image (mask image must be identical size to frame images)
			detector->setFrameMask(mask, width, height);

			// detect points on a frame
			auto points = detector->detectPointsOnFrame(frame, width, height);

			for (const auto& p : points)
			{
				// access each point (mpj_DetectedPoint) here
				p.x, p.y,...
			}
		\endcode
		\sa \ref mpj_DetectedPoint
	*/
	class mpj_PointDetector
	{
	public:
		static std::shared_ptr<mpj_PointDetector> create();

		//! Set mask using minimum/maximum angles.
		/*!
			\param width Width of the mask.
			\param height Height of the mask.
			\param minAngle Minimum pan/tilt angle.
			\param maxAngle Maximum pan/tilt angle.
			\param transform Transform used to map (x,y) to pan/tilt angles.
		*/
		virtual void setFrameMaskFromAngles(uint32_t width, uint32_t height, const std::shared_ptr<const mpj_Transform2d>& transform, const mpj_Angle& minAngle, const mpj_Angle& maxAngle) = 0;

		//! Set mask using default minimum/maximum angles.
		/*!
		\param width Width of the mask.
		\param height Height of the mask.
		*/
		void setFrameMaskFromDefaultAngles(uint32_t width, uint32_t height, const std::shared_ptr<const mpj_Transform2d>& transform)
		{
			setFrameMaskFromAngles(width, height, transform, mpj_Angle(-100, -30), mpj_Angle(100, 80));
		}

		//! Set mask using image.
		/*!
			\param image Mask image.
			\param width Width of the image.
			\param height Height of the image.
			\param channels Number of channels in the image. For single channel grayscale, it is 1. For RGB, it is 3.
			\param stride Stride of the image, a.k.a. bytes-per-line, of the image. It can be 0 if the stride can be calculated by width*channels.
			\param threshold If the value of the pixel is equal to or larger than the value, it will be recognized as a mask.
		*/
		virtual void setFrameMask(const void *image, uint32_t width, uint32_t height, uint32_t channels = 1, long stride = 0, uint8_t threshold = 1) = 0;

		//! Set mask using image.
		/*!
		\param image Mask image. 
		\param threshold If the value of the pixel is equal to or larger than the value, it will be recognized as a mask.
		*/
		void setFrameMask(const mpj_Image* image, uint8_t threshold = 1)
		{
			setFrameMask(image->getPixelPtr0(), image->getWidth(), image->getHeight(), image->getBytesPixePixel(), image->getStride(), threshold);
		}

		//! Set mask using image file.
		/*!
		\param maskImageFileName Mask image file name.
		\param threshold If the value of the pixel is equal to or larger than the value, it will be recognized as a mask.
		*/
		void setFrameMask(const _FSPFX path & maskImageFileName, uint8_t threshold = 1)
		{
		}

		//! Width of the frames in pixels.
		virtual uint32_t getWidth() const = 0;
		//! Height of hte frames in pixels.
		virtual uint32_t getHeight() const = 0;
		//! Determine whether the instance already has associated mask or not.
		virtual bool hasMask() const = 0;
		//! Pointer to the associated mask if available; otherwise nullptr.
		virtual const uint8_t* getMask() const = 0;

		using OnFrameCallback = std::function<void(uint8_t* frame, uint32_t width, uint32_t height, long stride)>;

		//! Detect bright points on a frame image.
		/*!
			\param image Frame image.
			\param width Width of the image.
			\param height Height of the image.
			\param channels Number of channels in the image. For single channel grayscale, it is 1. For RGB, it is 3.
			\param stride Stride of the image, a.k.a. bytes-per-line, of the image. It can be 0 if the stride can be calculated by width*channels.
			\param threshold If the value of the pixel is equal to or larger than the value, it will be recognized as a mask.
			\param onFrame If you want access to actual masked image used to detect points, the function is called back once on the detection process. You can even modify the pixels on the buffer to change the detection behavior but it's not recommended for normal usages.
		*/
		virtual std::vector<mpj_DetectedPoint> detectPointsOnFrame(const void *image, uint32_t width, uint32_t height, uint32_t channels = 1, long stride = 0, uint8_t threshold = 1, const OnFrameCallback& onFrame = nullptr) = 0;

		//! Detect bright points on a frame image.
		/*!
		\param mask Frame image.
		\param threshold If the value of the pixel is equal to or larger than the value, it will be recognized as a mask.
		\param onFrame If you want access to actual masked image used to detect points, the function is called back once on the detection process.
		*/
		std::vector<mpj_DetectedPoint> detectPointsOnFrame(const mpj_Image* image, uint8_t threshold = 1, const OnFrameCallback& onFrame = nullptr)
		{
			nullCheck(image);
			return detectPointsOnFrame(image->getPixelPtr0(), image->getWidth(), image->getHeight(), image->getBytesPixePixel(), image->getStride(), threshold, onFrame);
		}

		virtual void setZeta(float val) = 0;
		virtual float getZeta() const = 0;

	protected:
		mpj_PointDetector() {}
		virtual ~mpj_PointDetector() {}
	private:
		mpj_PointDetector(const mpj_PointDetector&) = delete;
		mpj_PointDetector& operator=(const mpj_PointDetector&) = delete;
	};
}

#endif // _mpj_pointerdetector_h_
