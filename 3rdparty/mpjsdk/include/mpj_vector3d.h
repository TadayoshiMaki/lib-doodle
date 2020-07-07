//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_vector3d.h
//! \brief 3D vector definitions
//---------------------------------------------------------------------------

#ifndef _mpj_vector3d_h_
#define _mpj_vector3d_h_

#include <algorithm>
#include <cmath>

#include "mpj_common.h"
#include "mpj_angle.h"

namespace mpjsdk
{
	//! 3D vector class definition.
	class mpj_Vec3
	{
	public:
		float x, y, z;

		//! Initialize all 0.
		constexpr mpj_Vec3() : x(0), y(0), z(0) {}

		//! Initialize the instance by duplicating the specified instance.
		/*! \param v Vector to duplicate. */
		constexpr mpj_Vec3(const mpj_Vec3& v) : x(v.x), y(v.y), z(v.z) {}

		//! Initialize with the specified coordinate.
		/*!
		\param x X-coordinate.
		\param y Y-coordinate.
		\param z Z-coordinate.
		*/
		constexpr mpj_Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		//! Initialize with the specified scalar value.
		/*!
		\param r Scalar value.
		*/
		explicit constexpr mpj_Vec3(float v) : x(v), y(v), z(v) {}

		mpj_Vec3& operator=(const mpj_Vec3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		constexpr bool operator==(const mpj_Vec3& r) const
		{
			return x == r.x && y == r.y && z == r.z;
		}
		constexpr bool operator!=(const mpj_Vec3& r) const
		{
			return !operator==(r);
		}

		constexpr bool isZero() const
		{
			return x == 0 && y == 0 && z == 0;
		}

		constexpr mpj_Vec3 operator+(const mpj_Vec3& r) const
		{
			return mpj_Vec3(x + r.x, y + r.y, z + r.z);
		}
		constexpr mpj_Vec3 operator-(const mpj_Vec3& r) const
		{
			return mpj_Vec3(x - r.x, y - r.y, z - r.z);
		}
		constexpr mpj_Vec3 operator*(const mpj_Vec3& r) const
		{
			return mpj_Vec3(x * r.x, y * r.y, z * r.z);
		}
		constexpr mpj_Vec3 operator/(const mpj_Vec3& r) const
		{
			return mpj_Vec3(x / r.x, y / r.y, z / r.z);
		}

		constexpr mpj_Vec3 operator+(float r) const
		{
			return mpj_Vec3(x + r, y + r, z + r);
		}
		constexpr mpj_Vec3 operator-(float r) const
		{
			return mpj_Vec3(x - r, y - r, z - r);
		}
		constexpr mpj_Vec3 operator*(float r) const
		{
			return mpj_Vec3(x * r, y * r, z * r);
		}
		constexpr mpj_Vec3 operator/(float r) const
		{
			return mpj_Vec3(x / r, y / r, z / r);
		}

		friend static constexpr mpj_Vec3 operator+(float r, const mpj_Vec3& v) { return v + r; }
		friend static constexpr mpj_Vec3 operator-(float r, const mpj_Vec3& v) { return v - r; }
		friend static constexpr mpj_Vec3 operator*(float r, const mpj_Vec3& v) { return v * r; }

		constexpr mpj_Vec3 operator-() const
		{
			return mpj_Vec3(-x, -y, -z);
		}

		mpj_Vec3& operator+=(const mpj_Vec3& r)
		{
			x += r.x;
			y += r.y;
			z += r.z;
			return *this;
		}
		mpj_Vec3& operator-=(const mpj_Vec3& r)
		{
			x -= r.x;
			y -= r.y;
			z -= r.z;
			return *this;
		}
		mpj_Vec3& operator*=(const mpj_Vec3& r)
		{
			x *= r.x;
			y *= r.y;
			z *= r.z;
			return *this;
		}
		mpj_Vec3& operator/=(const mpj_Vec3& r)
		{
			x /= r.x;
			y /= r.y;
			z /= r.z;
			return *this;
		}

		constexpr mpj_Vec3& operator*=(float r)
		{
			x *= r;
			y *= r;
			z *= r;
			return *this;
		}
		constexpr mpj_Vec3& operator/=(float r)
		{
			x /= r;
			y /= r;
			z /= r;
			return *this;
		}

		constexpr float length2() const
		{
			return x * x + y * y + z * z;
		}
		float length() const
		{
			return std::sqrt(length2());
		}
		mpj_Vec3 normalize() const
		{
			if (isZero())
				return *this;
			return *this / length();
		}

		constexpr float distance2(const mpj_Vec3& r) const
		{
			return (r - *this).length2();
		}
		const float distance(const mpj_Vec3& r) const
		{
			return std::sqrt(distance2(r));
		}

		mpj_Vec3 rotateXaxis(float angle) const
		{
			return mpj_Vec3(x, std::cos(angle) * y - std::sin(angle) * z, std::sin(angle) * y + std::cos(angle) * z);
		}
		mpj_Vec3 rotateYaxis(float angle) const
		{
			return mpj_Vec3(std::cos(angle) * x + std::sin(angle) * z, y, -std::sin(angle) * x + std::cos(angle) * z);
		}
		mpj_Vec3 rotateZaxis(float angle) const
		{
			return mpj_Vec3(std::cos(angle) * x - std::sin(angle) * y, std::sin(angle) * x + std::cos(angle) * y, z);
		}

		//! Inner product
		constexpr float dotProduct(const mpj_Vec3& r) const
		{
			return x * r.x + y * r.y + z * r.z;
		}

		//! Get perpendicular vector; a vector which is vertical to the plane defined by this vector and another.
		constexpr mpj_Vec3 getCrossingVector(const mpj_Vec3& r) const
		{
			return mpj_Vec3(
				y * r.z - z * r.y,
				z * r.x - x * r.z,
				x * r.y - y * r.x);
		}

		float angleRadian(const mpj_Vec3& r) const
		{
			auto n1 = this->normalize();
			auto n2 = r.normalize();
			auto dot = n1.dotProduct(n2);
			if (dot > 1.f) dot = 1.f;
			else if (dot < -1.f) dot = -1.f;
			return static_cast<float>(std::acos(dot));
		}

		float angleDegree(const mpj_Vec3& r) const
		{
			return mpj_Angle::radToDeg(angleRadian(r));
		}

		static mpj_Vec3 createUnitVectorFromPoint(const mpj_Angle& angle)
		{
			auto pan = mpj_Angle::degToRad(angle.pan);
			auto tilt = mpj_Angle::degToRad(angle.tilt);
			return mpj_Vec3(std::cos(tilt) * std::sin(pan), std::sin(tilt), std::cos(tilt) * std::cos(pan)).normalize();
		}

		void toAngleRadian(float& pan, float& tilt) const
		{
			auto n = normalize();
			tilt = std::asinf(n.y);
			if (std::abs(n.z) <= std::numeric_limits<float>::epsilon())
			{
				if (n.x > 0)
					pan = mpj_Angle::PI_float / 2;
				else if (n.x < 0)
					pan = -mpj_Angle::PI_float / 2;
				else
					pan = 0;
			}
			else
			{
				pan = std::atan2f(n.x, n.z);
			}
		}

		mpj_Angle toAngle() const
		{
			float pan, tilt;
			toAngleRadian(pan, tilt);
			return mpj_Angle(mpj_Angle::radToDeg(pan), mpj_Angle::radToDeg(tilt));
		}

	private:
		// basic tests against constexpr functions
		constexpr void tests()
		{
			// general testing of the class
			constexpr auto v = mpj_Vec3(1.f, 2.f, 3.f);
			static_assert(v.x == 1.f, "v.x is 1");
			static_assert(v.y == 2.f, "v.y is 2");
			static_assert(v.z == 3.f, "v.y is 3");
			static_assert(v.length2() == 14.f, "v.length2 == 1*1+2*2+3*3");
			constexpr auto v2 = mpj_Vec3(2.f);
			static_assert(v != v2, "v != v2");
			static_assert(v - v2 == mpj_Vec3(-1.f, 0.f, 1.f), "vector subtraction");
			static_assert(v + v2 == mpj_Vec3(3.f, 4.f, 5.f), "vector addition");
			static_assert(v * v2 == mpj_Vec3(2.f, 4.f, 6.f), "vector multiplication");
			static_assert(v / v2 == mpj_Vec3(.5f, 1.f, 1.5f), "vector division");
			static_assert(v.distance2(v2) == 2.f, "vector distance #1");
			static_assert(v.distance2(v2 * v2) == 14.f, "vector distance #2");

			// This is very platform specific, but it should be true to keep compatibility with float array.
			mpj_Vec3 a[3];
			static_assert(sizeof(a) == sizeof(float) * 3 * 3, "rough estimation of array continuity by checking size of array.");
		}
	};
}

#endif // _mpj_vector3d_h_
