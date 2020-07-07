//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_image.h
//! \brief Image definitions
//---------------------------------------------------------------------------

#ifndef _mpj_image_h_
#define _mpj_image_h_

#include <memory>
#include <functional>

#include "mpj_common.h"
#include "mpj_error.h"

namespace mpjsdk
{
	//! Image pixel format.
	enum class mpj_PixelFormat
	{
		Unknown = 0, //!< Unknown.
		Gray8 = 1, //!< Grayscale 8-bit.
		Gray16 = 2, //!< Grayscale 16-bit
		Rgb8 = 3, //!< RGB 24-bit (8-bit per channel)
		Rgba8 = 4, //!< RGBA 32-bit (8-bit per channel; premultiplied)
	};

	//! Image with raw data.
	/*!
	The following fragment illustrates how to create mpj_Image instances from raw images.
	\code{.cpp}
	// assume an 8-bit grayscale raw image on memory block
	const uint8_t* buffer = ...;
	uint32_t width = ...;
	uint32_t height = ...;
	auto format = mpj_PixelFormat::Gray8;
	int stride = static_cast<int>(width); // stride; bytes-per-line

	// create a mpj_Image, which references the existing raw image on a memory block
	// The buffer above should not be released during using the image.
	auto image = mpj_Image::create(buffer, stride, width, height, format);

	// create a mpj_Image, which "duplicates" the existing raw image on a memory block
	auto clone = mpj_Image::createClone(buffer, stride, width, height, format);

	// create just another clone of the clone
	auto anotherClone = mpj_Image::createClone(clone);
	\endcode
	\sa \ref mpj_Image::create, mpj_Image::createClone
	*/
	class mpj_Image
	{
	public:
		//! Get pointer to the raw image buffer.
		/*!
		In typical case, it's not what you need; use \ref getPixelPtr0 instead.
		If the image does not have its own buffer, the function return nullptr.
		It is not the pointer to the pixel at (0, 0) if the image is bottom-up.
		\sa \ref getPixelPtr0.
		*/
		virtual const uint8_t* getBuffer() const = 0;
		//! Get pointer to the raw image buffer.
		/*!
		In typical case, it's not what you need; use \ref getPixelPtr0 instead.
		If the image does not have its own buffer, the function return nullptr.
		It is not the pointer to the pixel at (0, 0) if the image is bottom-up.
		\sa \ref getPixelPtr0.
		*/
		virtual uint8_t* getBuffer() = 0;
		//! Get pointer to the pixel at (0, 0).
		/*!
		The value is different from \ref getBuffer if the image has negative stride (bottom-up);<br>
		If the stride (\ref getStride) is negative, the pointer obtained by the function is calculated by the following formula:
		\code{.cpp}
		pixelPtr0 = buffer + (height - 1) * stride;
		\endcode
		\sa \ref getBuffer, \ref getStride
		*/
		virtual const uint8_t* getPixelPtr0() const = 0;
		//! Get pointer to the pixel at (0, 0).
		/*!
		The value is different from \ref getBuffer if the image has negative stride (bottom-up);<br>
		If the stride (\ref getStride) is negative, the pointer obtained by the function is calculated by the following formula:
		\code{.cpp}
		pixelPtr0 = buffer + (height - 1) * stride;
		\endcode
		\sa \ref getBuffer, \ref getStride
		*/
		virtual uint8_t* getPixelPtr0() = 0;
		//! Image stride, a.k.a. bytes-per-line, including the paddings on the line end.
		/*!
			It can be negative if the image is bottom-up. See \ref getPixelPtr0 for more details.
		*/
		virtual long getStride() const = 0;
		//! Image width in pixels.
		virtual uint32_t getWidth() const = 0;
		//! Image height in pixels.
		virtual uint32_t getHeight() const = 0;
		//! Image pixel format.
		virtual mpj_PixelFormat getFormat() const = 0;
		//! Get bytes-per-pixel defined by \ref format.
		virtual uint32_t getBytesPixePixel() const = 0;

		//! Create a new mpj_Image.
		/*!
		\param width Image width in pixels.
		\param height Image height in pixels.
		\param format Image pixel format.
		\param stride Image stride, a.k.a. bytes-per-line, including the paddings on the line end. If 0 is specified, the stride will be calculated automatically.
		\sa \ref clone
		*/
		static std::shared_ptr<mpj_Image> create(uint32_t width, uint32_t height, mpj_PixelFormat format, int stride = 0);

		//! Create a mpj_Image based on existing image buffer.
		/*!
			\param image Pointer to buffer containing raw image.<br>
			The buffer should stay on memory during the life time of the newly created mpj_Image instance.
			\param stride Image stride, a.k.a. bytes-per-line, including the paddings on the line end.
			\param width Image width in pixels.
			\param height Image height in pixels.
			\param format Image pixel format.
			\sa \ref clone
		*/
		static std::shared_ptr<mpj_Image> create(void* image, int stride, uint32_t width, uint32_t height, mpj_PixelFormat format);

		//! Create a mpj_Image by copying existing image buffer.
		/*!
			\param image Pointer to buffer containing raw image. It can be nullptr if no image initialization is needed.
			\param stride Image stride, a.k.a. bytes-per-line, including the paddings on the line end.
			\param width Image width in pixels.
			\param height Image height in pixels.
			\param format Image pixel format.
			\param newStride
			For duplicate=true case, you can specify the stride for the newly created image. If 0 is specified, the new stride will be calculated automatically.<br>
			It can be negative if you want to store the image bottom-up on the newly created buffer. See \ref getPixelPtr0 for more details.
			\sa \ref create
		*/
		static std::shared_ptr<mpj_Image> create(const void* image, int stride, uint32_t width, uint32_t height, mpj_PixelFormat format, int newStride = 0);

		//! Clone the image.
		/*!
			\param newStride The stride for the newly created image. If 0 is specified, the new stride will be calculated automatically.
			\sa \ref create
		*/
		std::shared_ptr<mpj_Image> clone(int newStride = 0) const
		{
			return mpj_Image::create(getBuffer(), getStride(), getWidth(), getHeight(), getFormat(), newStride);
		}

		//! Initialize a new instance by loading an image file.
		/*!
			\param imageFileName File path to an image file.
			\param formatRequired Required pixel format.
		*/
		static std::shared_ptr<mpj_Image> createFromFile(const _FSPFX path & imageFileName, mpj_PixelFormat formatRequired = mpj_PixelFormat::Unknown, int stride = 0);

		//! Get pointer to the specified pixel.
		/*!
			\param x X-coordinate of the pixel.
			\param y Y-coordinate of the pixel.
			\return Pointer to the specified pixel.
		*/
		template<typename T = uint8_t> const T* getPixelPtr(uint32_t x, uint32_t y) const
		{
			static_assert(std::is_pod<T>::value, "T must be POD");
			return reinterpret_cast<const T*>(getPixelPtr0() + static_cast<long>(y) * getStride() + x * getBytesPixePixel());
		}

		//! Get pointer to the specified pixel.
		/*!
			\param x X-coordinate of the pixel.
			\param y Y-coordinate of the pixel.
			\return Pointer to the specified pixel.
		*/
		template<typename T = uint8_t> T* getPixelPtr(uint32_t x, uint32_t y)
		{
			static_assert(std::is_pod<T>::value, "T must be POD");
			return reinterpret_cast<T*>(getPixelPtr0() + static_cast<long>(y) * getStride() + x * getBytesPixePixel());
		}

		//! Get bytes-per-pixel from pixel format.
		/*!
		\param format pixel format.
		\return Bytes per pixel.
		*/
		static uint32_t getBytesPixePixel(mpj_PixelFormat format)
		{
			// FIXME: We should use better logic here :(
			return static_cast<uint32_t>(format);
		}

	protected:
		mpj_Image() {}
		virtual ~mpj_Image() {}
	};

	//! Simple image preview window
	/*!
	\ref mpj_ImagePreviewWindow provides a very simple and easy preview for \ref mpj_Image instance.
	\ref sendImage method is fully thread-safe and you can push image frames from any threads.

	To realize live camera preview, use \ref mpj_ThreadedFrameOperator::enablePreview rather than the class.

	The following fragment previews an image loaded from test.jpg:
	\code{.cpp}
	auto image = mpj_Image::createFromFile("test.jpg");

	auto preview = mpj_ImagePreviewWindow::create("Image Preview");
	preview->sendImage(image);

	...

	// To close the preview window, just release the instance.
	preview = nullptr;
	\endcode

	\sa \ref mpj_ThreadedFrameOperator, \ref mpj_Image
	*/
	class mpj_ImagePreviewWindow
	{
	public:
		/*!
		Open preview window with caption.
		\param caption Window title.
		*/
		static std::shared_ptr<mpj_ImagePreviewWindow> create(const std::string& caption);

		/*!
		Show an image on preview window.
		\param image to show.
		The method is fully thread-safe and you can send image from any threads.
		*/
		virtual void sendImage(const mpj_Image* image) = 0;

	protected:
		mpj_ImagePreviewWindow() {}
		virtual ~mpj_ImagePreviewWindow() {}
	};
}

#endif // _mpj_image_h_