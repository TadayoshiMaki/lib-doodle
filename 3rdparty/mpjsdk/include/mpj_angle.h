//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_angle.h
//! \brief Angle definitions
//---------------------------------------------------------------------------

#ifndef _mpj_angle_h_
#define _mpj_angle_h_

#include <algorithm>
#include <cmath>

#include "mpj_common.h"
#include "mpj_range.h"

namespace mpjsdk
{
#if !defined(DOXYGEN)
	//! Motor Pan/Tilt axis specification.
	enum class mpj_MotorSpec : size_t
	{
		Pan = 0, //!< Index for pan motor (0).
		Tilt = 1 //!< Index for tilt motor (1).
	};
#endif

	//! Angle class definition.
	class mpj_Angle
	{
	public:
		static constexpr float PAN_MAX = 180.f; //!< Maximum value for pan angle.
		static constexpr float PAN_MIN = -180.f; //!< Minimum value for pan angle.
		static constexpr float TILT_MAX = 90.f; //!< Maximum value for tilt angle.
		static constexpr float TILT_MIN = -90.f; //!< Minimum value for tilt angle.

		//! Valid pan angle range; [-180 180].
		static constexpr mpj_Range<float> validPanRange = mpj_Range<float>(PAN_MIN, PAN_MAX);
		//! Valid tilt angle range; [-90 90].
		static constexpr mpj_Range<float> validTiltRange = mpj_Range<float>(TILT_MIN, TILT_MAX);

		//! (0,0).
		static constexpr const mpj_Angle zero()
		{
			return mpj_Angle(0, 0);
		}

		//! Pan angle in degrees [-180 180].
		float pan;
		//! Tilt angle in degrees [-90 90].
		float tilt;

		//! Initialize all 0.
		constexpr mpj_Angle() : pan(0), tilt(0) {}

		//! Initialize the instance by duplicating the specified instance.
		/*! \param angle Angle to duplicate. */
		constexpr mpj_Angle(const mpj_Angle& angle) : pan(angle.pan), tilt(angle.tilt) {}

		//! Initialize with the specified parameters.
		/*!
		\param pan Pan angle [-180 180].
		\param tilt Tilt angle [-90 90].
		*/
		constexpr mpj_Angle(float pan, float tilt) : pan(pan), tilt(tilt) {}

		//! Duplicate the specified instance.
		/*! \param angle Angle to duplicate. */
		mpj_Angle& operator=(const mpj_Angle& angle)
		{
			pan = angle.pan;
			tilt = angle.tilt;
			return *this;
		}

		//! Compare with the specified instance.
		/*!
		\param r Angle to compare with.
		\return true if they are equal; otherwise false.
		*/
		constexpr bool operator==(const mpj_Angle& r) const
		{
			return pan == r.pan && tilt == r.tilt;
		}

		//! Compare with the specified instance.
		/*!
		\param r Angle to compare with.
		\return true if they are NOT equal; otherwise false.
		*/
		constexpr bool operator!=(const mpj_Angle& r) const
		{
			return !operator==(r);
		}

		//! Add two angles.
		/*!
		\param r Angle to add.
		\return The sum of the two angles.
		*/
		constexpr const mpj_Angle operator+(const mpj_Angle& r) const
		{
			return mpj_Angle(pan + r.pan, tilt + r.tilt);
		}

		//! Subtract an angle.
		/*!
		\param r Angle to subtract.
		\return The difference of the two angles.
		*/
		constexpr const mpj_Angle operator-(const mpj_Angle& r) const
		{
			return mpj_Angle(pan - r.pan, tilt - r.tilt);
		}

		//! Calculate square of the length of the angle.
		/*! \sa \ref length */
		constexpr const float length2() const
		{
			return pan * pan + tilt * tilt;
		}

		//! Calculate the length of the angle.
		/*! \sa \ref length2 */
		const float length() const
		{
			return std::sqrt(length2());
		}

		//! Calculate the square of the distance between angles.
		/*!
		\param r Angle to calculate distance with.
		\return The square of the distance of the two angles.
		*/
		constexpr const float distance2(const mpj_Angle& r) const
		{
			return (r - *this).length2();
		}

		//! Calculate the distance between angles.
		/*!
		\param r Angle to calculate distance with.
		\return The distance of the two angles.
		*/
		const float distance(const mpj_Angle& r) const
		{
			return std::sqrt(distance2(r));
		}

		//! Get one of pan/tilt angles according to the specification.
		/*!
		\param spec
		Specify which axis.
		*/
		constexpr float valueBySpec(mpj_MotorSpec spec) const { return spec == mpj_MotorSpec::Pan ? pan : tilt; }

		//! Set one of pan/tilt angles according to the specification.
		/*!
		\param spec
		Specify which axis.
		\param value
		Value for the angle.
		*/
		void setValueForSpec(float value, mpj_MotorSpec spec) { (spec == mpj_MotorSpec::Pan ? pan : tilt) = value; }

		// TODO: Add more methods here

		//! Verify parameters.
		/*! \return true if all the parameters are in valid range; otherwise false. */
		constexpr bool verifyParams() const
		{
			return validPanRange.isValueInRange(pan) && validTiltRange.isValueInRange(tilt);
		}

		//! Update parameters if they are out of range.
		constexpr void updateParamsIfInvalid()
		{
			pan = validPanRange.saturate(pan);
			tilt = validTiltRange.saturate(tilt);
		}

		//! Throw exception if some of the parameters are out of range.
		void throwIfInvalidParams() const;

		//! PI definition for double.
		constexpr static const double PI_double = 3.14159265358979323846;
		//! PI definition for float.
		constexpr static const float PI_float = static_cast<float>(PI_double);

		//! Degree to radian conversion.
		constexpr static float degToRad(float degree) { return static_cast<float>(degree * PI_float / 180.f); }
		//! Radian to degree conversion.
		constexpr static float radToDeg(float radian) { return static_cast<float>(radian * 180.f / PI_float); }

		//! Create \ref mpj_Angle from radian angle.
		/*!
		\param pan Pan angle in radian.
		\param tilt Tilt angle in radian.
		*/
		constexpr static mpj_Angle createFromRadianAngle(float pan, float tilt)
		{
			return mpj_Angle(radToDeg(pan), radToDeg(tilt));
		}

		//! Pan angle in radian.
		constexpr float panInRadian() const { return degToRad(pan); }
		//! Tilt angle in radian.
		constexpr float tiltInRadian() const { return degToRad(tilt); }
	};
}

#endif // _mpj_angle_h_
