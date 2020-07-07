//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_vector2d.h
//! \brief 2D vector definitions
//---------------------------------------------------------------------------

#ifndef _mpj_vector2d_h_
#define _mpj_vector2d_h_

#include <algorithm>
#include <cmath>

#include "mpj_common.h"
#include "mpj_angle.h"

namespace mpjsdk
{
	//! 2D vector class definition.
	class mpj_Vec2
	{
	public:
		float x, y;

		//! Initialize all 0.
		constexpr mpj_Vec2() : x(0), y(0) {}

		//! Initialize the instance by duplicating the specified instance.
		/*! \param v Vector to duplicate. */
		constexpr mpj_Vec2(const mpj_Vec2& v) : x(v.x), y(v.y) {}

		//! Initialize with the specified coordinate.
		/*!
		\param x X-coordinate.
		\param y Y-coordinate.
		*/
		constexpr mpj_Vec2(float x, float y) : x(x), y(y) {}

		//! Initialize with the specified scalar value.
		/*!
		\param r Scalar value.
		*/
		explicit constexpr mpj_Vec2(float v) : x(v), y(v) {}

		mpj_Vec2& operator=(const mpj_Vec2& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		constexpr bool operator==(const mpj_Vec2& r) const
		{
			return x == r.x && y == r.y;
		}
		constexpr bool operator!=(const mpj_Vec2& r) const
		{
			return !operator==(r);
		}

		constexpr bool isZero() const
		{
			return x == 0 && y == 0;
		}

		constexpr mpj_Vec2 operator+(const mpj_Vec2& r) const
		{
			return mpj_Vec2(x + r.x, y + r.y);
		}
		constexpr mpj_Vec2 operator-(const mpj_Vec2& r) const
		{
			return mpj_Vec2(x - r.x, y - r.y);
		}
		constexpr mpj_Vec2 operator*(const mpj_Vec2& r) const
		{
			return mpj_Vec2(x * r.x, y * r.y);
		}
		constexpr mpj_Vec2 operator/(const mpj_Vec2& r) const
		{
			return mpj_Vec2(x / r.x, y / r.y);
		}

		constexpr mpj_Vec2 operator+(float r) const
		{
			return mpj_Vec2(x + r, y + r);
		}
		constexpr mpj_Vec2 operator-(float r) const
		{
			return mpj_Vec2(x - r, y - r);
		}
		constexpr mpj_Vec2 operator*(float r) const
		{
			return mpj_Vec2(x * r, y * r);
		}
		constexpr mpj_Vec2 operator/(float r) const
		{
			return mpj_Vec2(x / r, y / r);
		}

		friend static constexpr mpj_Vec2 operator+(float r, const mpj_Vec2& v) { return v + r; }
		friend static constexpr mpj_Vec2 operator-(float r, const mpj_Vec2& v) { return v - r; }
		friend static constexpr mpj_Vec2 operator*(float r, const mpj_Vec2& v) { return v * r; }

		constexpr mpj_Vec2 operator-() const
		{
			return mpj_Vec2(-x, -y);
		}

		mpj_Vec2& operator+=(const mpj_Vec2& r)
		{
			x += r.x;
			y += r.y;
			return *this;
		}
		mpj_Vec2& operator-=(const mpj_Vec2& r)
		{
			x -= r.x;
			y -= r.y;
			return *this;
		}
		mpj_Vec2& operator*=(const mpj_Vec2& r)
		{
			x *= r.x;
			y *= r.y;
			return *this;
		}
		mpj_Vec2& operator/=(const mpj_Vec2& r)
		{
			x /= r.x;
			y /= r.y;
			return *this;
		}

		constexpr mpj_Vec2& operator*=(float r)
		{
			x *= r;
			y *= r;
			return *this;
		}
		constexpr mpj_Vec2& operator/=(float r)
		{
			x /= r;
			y /= r;
			return *this;
		}

		constexpr float length2() const
		{
			return x * x + y * y;
		}
		float length() const
		{
			return std::sqrt(length2());
		}
		mpj_Vec2 normalize() const
		{
			if (isZero())
				return *this;
			return *this / length();
		}

		constexpr float distance2(const mpj_Vec2& r) const
		{
			return (r - *this).length2();
		}
		const float distance(const mpj_Vec2& r) const
		{
			return std::sqrt(distance2(r));
		}

		//! Inner product
		constexpr float dotProduct(const mpj_Vec2& r) const
		{
			return x * r.x + y * r.y;
		}

		float angleRadian(const mpj_Vec2& r) const
		{
			return std::atan2(x * r.y - y * r.x, x * r.x + y * r.y);
		}

		float angleDegree(const mpj_Vec2& r) const
		{
			return mpj_Angle::radToDeg(angleRadian(r));
		}

	private:
		// basic tests against constexpr functions
		constexpr void tests()
		{
			// general testing of the class
			constexpr auto v = mpj_Vec2(1.f, 2.f);
			static_assert(v.x == 1.f, "v.x is 1");
			static_assert(v.y == 2.f, "v.y is 2");
			static_assert(v.length2() == 5.f, "v.length2 == 1*1+2*2");
			constexpr auto v2 = mpj_Vec2(2.f);
			static_assert(v != v2, "v != v2");
			static_assert(v - v2 == mpj_Vec2(-1.f, 0.f), "vector subtraction");
			static_assert(v + v2 == mpj_Vec2(3.f, 4.f), "vector addition");
			static_assert(v * v2 == mpj_Vec2(2.f, 4.f), "vector multiplication");
			static_assert(v / v2 == mpj_Vec2(.5f, 1.f), "vector division");
			static_assert(v.distance2(v2) == 1.f, "vector distance #1");
			static_assert(v.distance2(v2 * v2) == 13.f, "vector distance #2");

			// This is very platform specific, but it should be true to keep compatibility with float array.
			mpj_Vec2 a[3];
			static_assert(sizeof(a) == sizeof(float) * 2 * 3, "rough estimation of array continuity by checking size of array.");
		}
	};
}

#endif // _mpj_vector2d_h_
