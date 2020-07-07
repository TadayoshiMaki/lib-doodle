//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_quaternion.h
//! \brief Quaternion support
//---------------------------------------------------------------------------

#ifndef _mpj_quaternion_h_
#define _mpj_quaternion_h_

#include <cmath>

namespace mpjsdk
{
	//! Quaternion support.
	class mpj_Quaternion
	{
	public:
		float x, y, z, w;

		//! Initialize the instance.
		constexpr mpj_Quaternion() : x(0), y(0), z(0), w(1) { }

		//! Initialize the instance.
		constexpr mpj_Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }

		//! Initialize the instance.
		constexpr mpj_Quaternion(const mpj_Quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) { }

		//! Initialize the instance by angle and axis.
		/*!
		\param angleDegree Rotation angle in degrees.
		\param axis Axis about which to rotate the quaternion.
		*/
		mpj_Quaternion(float angleDegree, const mpj_Vec3& axis)
		{
			auto angle = mpj_Angle::degToRad(angleDegree);
			auto length = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

			const auto epsilon = 0.0000001;
			if (length < epsilon)
			{
				// ~zero length axis, so reset rotation to zero.
				x = y = z = 0.f;
				w = 1.f;
				return;
			}

			auto m = std::sinf(0.5f * angle) / length;
			x = axis.x * m;
			y = axis.y * m;
			z = axis.z * m;
			w = std::cosf(0.5f * angle);
		}

		mpj_Quaternion(float angle1, const mpj_Vec3& axis1, float angle2, const mpj_Vec3& axis2, float angle3, const mpj_Vec3& axis3)
		{
			auto q1 = mpj_Quaternion(angle1, axis1);
			auto q2 = mpj_Quaternion(angle2, axis2);
			auto q3 = mpj_Quaternion(angle3, axis3);
			*this = q1 * q2 * q3;
		}

		mpj_Quaternion& operator=(const mpj_Quaternion& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
			return *this;
		}

		constexpr bool operator==(const mpj_Quaternion& q) const
		{
			return x == q.x && y == q.y && z == q.z && w == q.w;
		}
		constexpr bool operator!=(const mpj_Quaternion& q) const
		{
			return !operator==(q);
		}
		constexpr mpj_Quaternion operator*(float r) const
		{
			return mpj_Quaternion(x * r, y * r, z * r, w * r);
		}
		constexpr mpj_Quaternion operator/(float r) const
		{
			return mpj_Quaternion(x / r, y / r, z / r, w / r);
		}
		constexpr mpj_Quaternion operator+(const mpj_Quaternion& q) const
		{
			return mpj_Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
		}
		constexpr mpj_Quaternion operator-(const mpj_Quaternion& q) const
		{
			return mpj_Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
		}
		constexpr mpj_Quaternion operator*(const mpj_Quaternion& q) const
		{
			return mpj_Quaternion(
				q.w * x + q.x * w + q.y * z - q.z * y,
				q.w * y - q.x * z + q.y * w + q.z * x,
				q.w * z + q.x * y - q.y * x + q.z * w,
				q.w * w - q.x * x - q.y * y - q.z * z);
		}
		constexpr mpj_Quaternion operator/(const mpj_Quaternion& q) const
		{
			return *this * q.inverse();
		}
		constexpr mpj_Vec3 operator*(const mpj_Vec3& v) const
		{
			// compatible to nVidia SDK implementation
			mpj_Vec3 qvec(x, y, z);
			auto uv = qvec.getCrossingVector(v);    // uv = qvec ^ v;
			auto uuv = qvec.getCrossingVector(uv);    // uuv = qvec ^ uv;
			uv *= (2.0f * w);
			uuv *= 2.0f;
			return v + uv + uuv;
		}

		friend static constexpr mpj_Quaternion operator*(float r, const mpj_Quaternion& q) { return q * r; }

		constexpr mpj_Quaternion operator-() const
		{
			return mpj_Quaternion(-x, -y, -z, -w);
		}

		mpj_Quaternion& operator+=(const mpj_Quaternion& q)
		{
			x += q.x;
			y += q.y;
			z += q.z;
			w += q.w;
			return *this;
		}
		mpj_Quaternion& operator-=(const mpj_Quaternion& q)
		{
			x -= q.x;
			y -= q.y;
			z -= q.z;
			w -= q.w;
			return *this;
		}
		mpj_Quaternion& operator*=(const mpj_Quaternion& q)
		{
			*this = *this * q;
			return *this;
		}
		mpj_Quaternion& operator/=(const mpj_Quaternion& q)
		{
			*this = *this / q;
			return *this;
		}
		mpj_Quaternion& operator*=(float r)
		{
			x *= r;
			y *= r;
			z *= r;
			w *= r;
			return *this;
		}
		mpj_Quaternion& operator/=(float r)
		{
			x /= r;
			y /= r;
			z /= r;
			w /= r;
			return *this;
		}

		//! Get the square of the length of the quaternion.
		constexpr float length2() const
		{
			return x * x + y * y + z * z + w * w;
		}
		//! Get the length of the quaternion.
		float length() const
		{
			return std::sqrt(length2());
		}
		//! Determine whether the quaternion has rotation or not.
		constexpr bool isNoRotation() const
		{
			return x == 0 && y == 0 && z == 0 && w == 1.f;
		}
		//! Determine whether the quaternion is all-zero or not.
		constexpr bool isZero() const
		{
			return x == 0 && y == 0 && z == 0 && w == 0;
		}
		//! Get conjugate of the quaternion.
		constexpr mpj_Quaternion conjugate() const
		{
			return mpj_Quaternion(-x, -y, -z, w);
		}
		//! Get inverse of the quaternion.
		constexpr mpj_Quaternion inverse() const
		{
			return conjugate() / length2();
		}
		//! Get normalized quaternion.
		mpj_Quaternion normalize() const
		{
			if (isZero())
				return *this;
			auto len = length();
			return mpj_Quaternion(x / len, y / len, z / len, w / len);
		}
	};
}

#endif // _mpj_quaternion_h_
