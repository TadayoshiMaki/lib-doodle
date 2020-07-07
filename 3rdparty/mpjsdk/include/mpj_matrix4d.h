//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_matrix4d.h
//! \brief 3D vector definitions
//---------------------------------------------------------------------------

#ifndef _mpj_matrix4d_h_
#define _mpj_matrix4d_h_

#include <algorithm>
#include <cmath>
#include <limits>
#include <array>

#include "mpj_common.h"
#include "mpj_vector3d.h"
#include "mpj_quaternion.h"
#include "mpj_angle.h"

namespace mpjsdk
{
	//! 3D vector class definition.
	class mpj_Mat4
	{
	public:
		static const size_t rows = 4, cols = 4;
		typedef std::array<float, rows * cols> Array;
		typedef std::array<float, 9> Array9;
		Array m;

		//! Initialize the instance with identity matrix
		constexpr mpj_Mat4() : m {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f }
		{
		}

		//! Initialize the instance by duplicating the specified instance.
		/*! \param v Vector to duplicate. */
		constexpr mpj_Mat4(const mpj_Mat4& v) : m { v.m[0], v.m[1], v.m[2], v.m[3], v.m[4], v.m[5], v.m[6], v.m[7], v.m[8], v.m[9], v.m[10], v.m[11], v.m[12], v.m[13], v.m[14], v.m[15] }
		{
		}

		//! Initialize the instance by duplicating the specified instance.
		constexpr mpj_Mat4(
			float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13,
			float a20, float a21, float a22, float a23,
			float a30, float a31, float a32, float a33) : m {
				a00, a01, a02, a03,
				a10, a11, a12, a13,
				a20, a21, a22, a23,
				a30, a31, a32, a33 }
		{
		}

		//! Initialize the instance by duplicating the specified instance.
		constexpr mpj_Mat4(
			float a00, float a01, float a02,
			float a10, float a11, float a12,
			float a20, float a21, float a22) : m{
				a00, a01, a02, 0.f,
				a10, a11, a12, 0.f,
				a20, a21, a22, 0.f,
				0.f, 0.f, 0.f, 1.f }
		{
		}

		//! Initialize the instance by duplicating the specified instance.
		explicit mpj_Mat4(const float* v)
		{
			std::memcpy(&m[0], v, sizeof(m));
		}

		//! Initialize the instance by duplicating the specified instance.
		explicit constexpr mpj_Mat4(const Array& v) : m(v)
		{
		}

		//! Initialize the instance by duplicating the specified instance.
		explicit constexpr mpj_Mat4(const Array9& v) : m{
			v[0], v[1], v[2], 0,
			v[3], v[4], v[5], 0,
			v[6], v[7], v[8], 0,
			0, 0, 0, 1.f }
		{
		}

		//! Initialize with the specified scalar value.
		/*!
		\param v Scalar value.
		*/
		explicit constexpr mpj_Mat4(float v) : m { v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v }
		{
		}

		//! Create a new \ref mpj_Mat4 instance from a \ref mpj_Quaternion instance.
		/*!
		\param q Quaternion.
		*/
		static constexpr mpj_Mat4 createFromQuaternion(const mpj_Quaternion& q)
		{
			auto len2 = q.length2();
			if (len2 <= (std::numeric_limits<decltype(len2)>::min)())
				return mpj_Mat4::identity();
			
			// normalize quat if required.
			// We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
			// That is a square of a square root, so it is possible to avoid that
			auto rlength2 = 2.0 / len2;

			// Source: Gamasutra, Rotating Objects Using Quaternions
			// http://www.gamasutra.com/features/19980703/quaternions_01.htm

			// calculate coefficients
			auto x2 = rlength2 * q.x;
			auto y2 = rlength2 * q.y;
			auto z2 = rlength2 * q.z;

			auto xx = q.x * x2;
			auto xy = q.x * y2;
			auto xz = q.x * z2;

			auto yy = q.y * y2;
			auto yz = q.y * z2;
			auto zz = q.z * z2;

			auto wx = q.w * x2;
			auto wy = q.w * y2;
			auto wz = q.w * z2;

			return mpj_Mat4(
				static_cast<float>(1.f - (yy + zz)),
				static_cast<float>(xy + wz),
				static_cast<float>(xz - wy),
				0,
				static_cast<float>(xy - wz),
				static_cast<float>(1.f - (xx + zz)),
				static_cast<float>(yz + wx),
				0,
				static_cast<float>(xz + wy),
				static_cast<float>(yz - wx),
				static_cast<float>(1.f - (xx + yy)),
				0,
				0, 0, 0, 1);
		}

		//! Create rotation matrix from a \ref mpj_Angle instance.
		/*!
		\param angle Angle in degrees.
		*/
		static mpj_Mat4 createFromAngle(const mpj_Angle& angle)
		{
			auto pan = angle.panInRadian();
			auto tilt = angle.tiltInRadian();
			return mpj_Mat4(
				std::cos(pan),
				std::sin(pan)*std::sin(-tilt),
				std::sin(pan)*std::cos(-tilt),
				0.0,
				0.0,
				std::cos(-tilt),
				-std::sin(-tilt),
				0.0,
				-std::sin(pan),
				std::cos(pan)*std::sin(-tilt),
				std::cos(pan)*std::cos(-tilt),
				0.0,
				0.0,
				0.0,
				0.0,
				1.0);
		}

		constexpr mpj_Mat4& operator=(const mpj_Mat4& v)
		{
			for (size_t i = 0; i < m.size(); i++)
				m[i] = v.m[i];
			return *this;
		}

		constexpr bool operator==(const mpj_Mat4& r) const
		{
			for (size_t i = 0; i < m.size(); i++)
				if (m[i] != r.m[i])
					return false;
			return true;
		}
		constexpr bool operator!=(const mpj_Mat4& r) const
		{
			return !operator==(r);
		}

		static constexpr mpj_Mat4 zero()
		{
			return mpj_Mat4(0.f);
		}
		constexpr bool isZero() const
		{
			return *this == zero();
		}

		static constexpr mpj_Mat4 identity()
		{
			return mpj_Mat4();
		}
		constexpr bool isIdentity() const
		{
			return *this == identity();
		}

		constexpr mpj_Mat4 operator*(const mpj_Mat4& r) const
		{
			return mpj_Mat4(
				innerProduct(*this, r, 0, 0),
				innerProduct(*this, r, 0, 1),
				innerProduct(*this, r, 0, 2),
				innerProduct(*this, r, 0, 3),
				innerProduct(*this, r, 1, 0),
				innerProduct(*this, r, 1, 1),
				innerProduct(*this, r, 1, 2),
				innerProduct(*this, r, 1, 3),
				innerProduct(*this, r, 2, 0),
				innerProduct(*this, r, 2, 1),
				innerProduct(*this, r, 2, 2),
				innerProduct(*this, r, 2, 3),
				innerProduct(*this, r, 3, 0),
				innerProduct(*this, r, 3, 1),
				innerProduct(*this, r, 3, 2),
				innerProduct(*this, r, 3, 3));
		}

		mpj_Mat4& operator*=(const mpj_Mat4& r)
		{
			*this = (*this) * r;
			return *this;
		}

		constexpr const float operator()(size_t row, size_t col) const { return m[row * cols + col]; }
		float& operator()(size_t row, size_t col) { return m[row * cols + col]; }

		constexpr mpj_Vec3 operator*(const mpj_Vec3& v) const
		{
			auto d = 1.0f / (me(3, 0) * v.x + me(3, 1) * v.y + me(3, 2) * v.z + me(3, 3));
			return mpj_Vec3(
				(me(0, 0) * v.x + me(0, 1) * v.y + me(0, 2) * v.z + me(0, 3)) * d,
				(me(1, 0) * v.x + me(1, 1) * v.y + me(1, 2) * v.z + me(1, 3)) * d,
				(me(2, 0) * v.x + me(2, 1) * v.y + me(2, 2) * v.z + me(2, 3)) * d);
		}

		mpj_Mat4 inverse() const;

		constexpr bool is4x3() const
		{
			return me(0, 3) == 0.0f && me(1, 3) == 0.0f && me(2, 3) == 0.0f && me(3, 3) == 1.0f;
		}

		constexpr bool is3x3() const
		{
			return is4x3() && me(3, 0) == 0.0f && me(3, 1) == 0.0f && me(3, 2) == 0.0f;
		}

		mpj_Vec3 getScale() const
		{
			return mpj_Vec3(
				mpj_Vec3(me(0, 0), me(1, 0), me(2, 0)).length(),
				mpj_Vec3(me(0, 1), me(1, 1), me(2, 1)).length(),
				mpj_Vec3(me(0, 2), me(1, 2), me(2, 2)).length());
		}

		constexpr mpj_Mat4 scale(float x, float y, float z) const
		{
			return mpj_Mat4(
				m[0]  * x, m[1]  * y, m[2]  * z, m[3],
				m[4]  * x, m[5]  * y, m[6]  * z, m[7],
				m[8]  * x, m[9]  * y, m[10] * z, m[11],
				m[12] * x, m[13] * y, m[14] * z, m[15]);
		}

		constexpr mpj_Mat4 scale(const mpj_Vec3& vec) const
		{
			return scale(vec.x, vec.y, vec.z);
		}

		mpj_Quaternion getRotate() const;

	private:
		// NOTE: they're completely identical routine to operator() but they're useful when using "this"; (*this)(r,c) is painful.
		constexpr const float me(size_t row, size_t col) const { return (*this)(row, col); }
		float& me(size_t row, size_t col) { return (*this)(row, col); }

		// basic tests against constexpr functions
		static constexpr void tests()
		{
			// general testing of the class
			constexpr auto v = mpj_Vec3(1.f, 2.f, 3.f);
			constexpr auto v2 = mpj_Vec3(2.f);
			constexpr auto ind = mpj_Mat4::identity();
			static_assert(ind * v == v, "matrix identity multiplication test");

			// This is very platform specific, but it should be true
			mpj_Mat4 a[3];
			static_assert(sizeof(a) == sizeof(float) * 16 * 3, "rough estimation of array continuity by checking size of array.");
		}

		static constexpr float innerProduct(const mpj_Mat4& a, const mpj_Mat4& b, size_t r, size_t c)
		{
			return a(r, 0) * b(0, c) + a(r, 1) * b(1, c) + a(r, 2) * b(2, c) + a(r, 3) * b(3, c);
		}
	};
}

#endif // _mpj_matrix4d_h_
