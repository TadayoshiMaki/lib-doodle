//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_geodesicdome.h
//! \brief Geodesic dome definitions
//---------------------------------------------------------------------------

#ifndef _mpj_geodesicdome_h_
#define _mpj_geodesicdome_h_

#include "mpj_common.h"
#include "mpj_error.h"
#include "mpj_vector3d.h"
#include "mpj_matrix4d.h"
#include "mpj_quaternion.h"
#include "mpj_angle.h"
#include "mpj_depth.h"
#include <cmath>
#include <filesystem>

namespace mpjsdk
{
	//! Mesh index used with \ref mpj_GeodesicDome. 
	struct mpj_MeshIndex
	{
		//!< Initialize an instance.
		mpj_MeshIndex() : a(0), b(0), c(0) {}
		//!< Initialize an instance with the specified indices.
		mpj_MeshIndex(uint32_t a, uint32_t b, uint32_t c) : a(a), b(b), c(c) {}

		uint32_t a; //!< Index A
		uint32_t b; //!< Index B
		uint32_t c; //!< Index C

	private:
		void test()
		{
			mpj_MeshIndex mi[2];
			static_assert(sizeof(mi) == sizeof(uint32_t) * 3 * 2, "mpj_MeshIndex size check.");
		}
	};

	//! Cloud point, which extends \ref mpj_Vec3 with a validity flag.
	struct mpj_CloudPoint
	{
		//!< Initialize an instance.
		constexpr mpj_CloudPoint() : point(), isValid(false) {}

		//!< Initialize an instance.
		constexpr mpj_CloudPoint(float x, float y, float z) : point(x, y, z), isValid(false) {}

		//!< Initialize an instance.
		constexpr mpj_CloudPoint(const mpj_Vec3& p) : point(p), isValid(false) {}

		mpj_Vec3 point; //!< Could point coodinates.
		bool isValid; //!< Whether the point is valid or not.

	private:
		void test()
		{
			mpj_CloudPoint cp[2];
			static_assert(sizeof(cp) == 16 * 2, "mpj_CloudPoint size check.");
		}
	};

	//! Plane detection result.
	struct mpj_PlaneInfo
	{
		mpj_PlaneInfo() : pixelsPerMillimeter(1.0f)
		{
		}

		float pixelsPerMillimeter;
		mpj_Vec3 normal;
		mpj_Vec3 center;
		std::vector<mpj_Vec3> convex;
	};

	//! Geodesic dome implementation.
	class mpj_GeodesicDome
	{
	public:
		//! Default geodesic dome level.
		static const int DefaultGeodesicDomeLevel = 6;

		explicit mpj_GeodesicDome(int level = DefaultGeodesicDomeLevel) : needUpdateVectors(true), radius(1.f)
		{
			setLevel(level);
		}

		mpj_GeodesicDome(const mpj_Vec3& center, const std::vector<mpj_CloudPoint>& cloud, int level = DefaultGeodesicDomeLevel) : needUpdateVectors(true), radius(1.f)
		{
			setLevel(level);
			updateVectorsIfNeeded();
			this->cloud = cloud;
		}

		explicit mpj_GeodesicDome(const _FSPFX path& fileName, bool throwOnError, int level = DefaultGeodesicDomeLevel)
		{
			loadCloudFromFile(fileName, throwOnError, level);
		}

		const std::vector<mpj_Vec3> getVectors() const { return vecs; }
		const std::vector<mpj_MeshIndex> getMeshIndices() const { return meshIndices; }
		const std::vector<mpj_CloudPoint> getCloud() const { return cloud; }

		bool isValid() const { return cloud.size() ? true : false; }

		//! Get the current level.
		int getLevel() const { return level; }

		//! Set the level.
		/*!
			Please note that the level set is not immediately reflected to the vectors.
			If you want to update the vectors immediately, call \ref updateVectorsIfNeeded.
		*/
		void setLevel(int level)
		{
			if (this->level == level)
				return;
			this->level = level <= 0 ? 1 : level;
			maxEdgeLength = 1.f / static_cast<float>(std::pow(2, level - 1));
			needUpdateVectors = true;
		}

		//! Get the center point.
		const mpj_Vec3& getCenter() const { return center; }

		//! Set the center point.
		/*!
		Please note that the level set is not immediately reflected to the vectors.
		If you want to update the vectors immediately, call \ref updateVectorsIfNeeded.
		*/
		void setCenter(const mpj_Vec3& center)
		{
			if (this->center == center)
				return;
			this->center = center;
			needUpdateVectors = true;
		}

		//! Reset the parameters.
		/*!
		Please note that the level set is not immediately reflected to the vectors.
		If you want to update the vectors immediately, call \ref updateVectorsIfNeeded.
		*/
		void reset()
		{
			needUpdateVectors = true;
			radius = 1.f;
			level = 1;
			maxEdgeLength = 1.f;
			vecs.clear();
			meshIndices.clear();
			center = mpj_Vec3();
			cloud.clear();
		}

		void updateVectorsIfNeeded()
		{
			if (!needUpdateVectors)
				return;

			meshIndices.clear();

			// edge length = golden ratio
			const auto sqrt5 = std::sqrt(5.0f);
			const auto phi = (1.0f + sqrt5) * 0.5f;
			// circumscribed radius
			const auto cRadius = std::sqrt(10.0f + (2.0f * sqrt5)) / (4.0f * phi);
			// now we define constants which will define our Icosahedron's vertices
			auto a = static_cast<float>(radius / cRadius * 0.5f);
			auto b = static_cast<float>(radius / cRadius / (2.0f * phi));

			// vertices of the Icosahedron:
			mpj_Vec3 v[12];
			v[0] = center + mpj_Vec3(0, b, -a);
			v[1] = center + mpj_Vec3(b, a, 0);
			v[2] = center + mpj_Vec3(-b, a, 0);
			v[3] = center + mpj_Vec3(0, b, a);
			v[4] = center + mpj_Vec3(0, -b, a);
			v[5] = center + mpj_Vec3(-a, 0, b);
			v[6] = center + mpj_Vec3(0, -b, -a);
			v[7] = center + mpj_Vec3(a, 0, -b);
			v[8] = center + mpj_Vec3(a, 0, b);
			v[9] = center + mpj_Vec3(-a, 0, -b);
			v[10] = center + mpj_Vec3(b, -a, 0);
			v[11] = center + mpj_Vec3(-b, -a, 0);

			vecs = std::vector<mpj_Vec3>(v, v + 12);

			// draw the icosahedron
			icosahedron(v[0], v[1], v[2], 0, 1, 2);
			icosahedron(v[3], v[2], v[1], 3, 2, 1);
			icosahedron(v[3], v[4], v[5], 3, 4, 5);
			icosahedron(v[3], v[8], v[4], 3, 8, 4);
			icosahedron(v[0], v[6], v[7], 0, 6, 7);
			icosahedron(v[0], v[9], v[6], 0, 9, 6);
			icosahedron(v[4], v[10], v[11], 4, 10, 11);
			icosahedron(v[6], v[11], v[10], 6, 11, 10);
			icosahedron(v[2], v[5], v[9], 2, 5, 9);
			icosahedron(v[11], v[9], v[5], 11, 9, 5);
			icosahedron(v[1], v[7], v[8], 1, 7, 8);
			icosahedron(v[10], v[8], v[7], 10, 8, 7);
			icosahedron(v[3], v[5], v[2], 3, 5, 2);
			icosahedron(v[3], v[1], v[8], 3, 1, 8);
			icosahedron(v[0], v[2], v[9], 0, 2, 9);
			icosahedron(v[0], v[7], v[1], 0, 7, 1);
			icosahedron(v[6], v[9], v[11], 6, 9, 11);
			icosahedron(v[6], v[10], v[7], 6, 10, 7);
			icosahedron(v[4], v[11], v[5], 4, 11, 5);
			icosahedron(v[4], v[8], v[10], 4, 8, 10);

			needUpdateVectors = false;
		}

		void updateCloud(const mpj_DepthPoints& depthPoints, const mpj_Mat4& rotationMatrix)
		{
			updateCloud(depthPoints.getWidth(), depthPoints.getHeight(), depthPoints.getPoints(), rotationMatrix);
		}

		void updateCloud(uint32_t width, uint32_t height, const std::vector<mpj_Vec3>& points, const mpj_Mat4& rotationMatrix)
		{
			updateVectorsIfNeeded();

			auto w = static_cast<int>(width);
			auto h = static_cast<int>(height);

			for (int i = 0; i < vecs.size(); i++)
			{
				const auto& v = vecs[i];
				auto convertVector = mpj_DepthPoints::worldToDepth(rotationMatrix * v);
				auto x = static_cast<int>(convertVector.x);
				auto y = static_cast<int>(convertVector.y);

				if (x >= 0 && x < w && y >= 0 && y < h)
				{
					auto& pt = points[y * w + x];
					if (!pt.isZero())
					{
						auto cos = v.dotProduct(pt) / (pt.length() * v.length());
						if (cos > 0.85f)
						{
							if (cloud[i].isValid)
							{
								if (cloud[i].point.length() > pt.length())
									cloud[i].point = pt;
							}
							else
							{
								cloud[i].point = pt;
								cloud[i].isValid = true;
							}
						}
					}
				}
			}
		}

		void smoothCloud();

		void saveCloudToFile(const _FSPFX path& fileName) const;

		void loadCloudFromFile(const _FSPFX path& fileName, bool throwOnError, int level = DefaultGeodesicDomeLevel);

		//! Convert cloud to vertices.
		/*!
		\param vertices Buffer to receive vertices.
		*/
		void cloudToVertices(std::vector<mpj_Vec3>& vertices) const;

		//! Extract points from point cloud.
		/*!
			\param x Specify area to extract.
			\param y Specify area to extract.
			\param width Specify area to extract.
			\param height Specify area to extract.
			\param rotation Rotation matrix.
			\param intrinsic Intrinsic matrix for the target such as projector or depth-sensor.
			\param translateIfNeeded Translation for the target if needed.
			\return Extracted points.
		*/
		std::vector<mpj_Vec3> extractPoints(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const mpj_Mat4& rotation, const mpj_Mat4& intrinsic, const mpj_Vec3& translateIfNeeded = mpj_Vec3()) const
		{
			auto fx = intrinsic.m[0];
			auto fy = intrinsic.m[5];
			auto cx = intrinsic.m[2];
			auto cy = intrinsic.m[6];

			auto exMatInv = rotation;
			exMatInv(0, 3) = translateIfNeeded.x;
			exMatInv(1, 3) = translateIfNeeded.y;
			exMatInv(2, 3) = translateIfNeeded.z;
			exMatInv = exMatInv.inverse();
			
			auto xend = x + width;
			auto yend = y + height;

			std::vector<mpj_Vec3> ret;
			for (size_t i = 0; i < cloud.size(); i++)
			{
				auto& pt = cloud[i].point;
				auto cv = exMatInv * pt;
				cv.x = fx * cv.x / cv.z + cx;
				cv.y = fy * cv.y / cv.z + cy;
				if (cv.x >= x && cv.x < xend && cv.y >= y && cv.y < yend && cv.z > 0.f)
					ret.push_back(pt);
			}
			return ret;
		}

		//! Detect plane on point cloud.
		/*!
		\param lastPlaneInfo On return, it contains the information about the plane detected.
		\param x Specify area to extract.
		\param y Specify area to extract.
		\param width Specify area to extract.
		\param height Specify area to extract.
		\param rotation Rotation matrix.
		\param intrinsic Intrinsic matrix for the target such as projector or depth-sensor.
		\param translateIfNeeded Translation for the target if needed.
		\return true if a plane is detected; otherwise false.
		*/
		bool detectPlane(mpj_PlaneInfo& lastPlaneInfo, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const mpj_Mat4& rotation, const mpj_Mat4& intrinsic, const mpj_Vec3& translateIfNeeded = mpj_Vec3()) const
		{
			auto points = extractPoints(x, y, width, height, rotation, intrinsic, translateIfNeeded);
			if (points.size() == 0)
				return false;
			return detectPlane(lastPlaneInfo, points);
		}

		//! Detect plane on point cloud.
		/*!
		\param lastPlaneInfo On return, it contains the information about the plane detected.
		\return true if a plane is detected; otherwise false.
		*/
		bool detectPlane(mpj_PlaneInfo& lastPlaneInfo, const std::vector<mpj_Vec3>& pointsOnPlane) const;

		bool findMostClosestDistPoint(const mpj_Vec3& from, const mpj_Vec3& to, float& minAngle, mpj_Vec3& minPoint) const
		{
			if (!cloud.size())
			return false;

			auto minA = (std::numeric_limits<float>::max)();
			mpj_Vec3 minP;
			for (const auto& p : cloud)
			{
				auto ap = p.point - from;
				auto angle = to.angleDegree(ap.normalize());
				if (minA > angle)
				{
					minA = angle;
					minP = p.point;
				}
			}
			minAngle = minA;
			minPoint = minP;
			return true;
		}

	private:
		bool needUpdateVectors;
		float radius;
		int level;
		float maxEdgeLength;
		mpj_Vec3 center;
		std::vector<mpj_Vec3> vecs;
		std::vector<mpj_MeshIndex> meshIndices;
		std::vector<mpj_CloudPoint> cloud;

		void icosahedron(const mpj_Vec3& a, const mpj_Vec3& b, const mpj_Vec3& c, const uint32_t indexA, const uint32_t indexB, const uint32_t indexC)
		{
			auto edge1 = a - b;
			auto edge2 = b - c;
			auto edge3 = c - a;

			auto mag1 = edge1.length();
			auto mag2 = edge2.length();
			auto mag3 = edge3.length();

			// if all edges are short enough
			if (mag1 < maxEdgeLength && mag2 < maxEdgeLength && mag3 < maxEdgeLength)
			{
				//register triangle
				meshIndices.push_back(mpj_MeshIndex(indexA, indexB, indexC));
			}
			else // otherwise subdivide and recurse
			{
				// find edge midpoints
				const auto ab = midpointOnSphere(a, b);
				const auto bc = midpointOnSphere(b, c);
				const auto ca = midpointOnSphere(c, a);
				vecs.push_back(ab);
				auto indexAB = uint32_t(vecs.size() - 1);
				vecs.push_back(bc);
				auto indexBC = uint32_t(vecs.size() - 1);
				vecs.push_back(ca);
				auto indexCA = uint32_t(vecs.size() - 1);

				// create 4 sub-divided triangles
				icosahedron(a, ab, ca, indexA, indexAB, indexCA);
				icosahedron(ab, b, bc, indexAB, indexB, indexBC);
				icosahedron(ca, bc, c, indexCA, indexBC, indexC);
				icosahedron(ab, bc, ca, indexAB, indexBC, indexCA);
			}
			cloud.resize(vecs.size());
		}

		mpj_Vec3 midpointOnSphere(const mpj_Vec3& a, const mpj_Vec3& b)
		{
			auto midpoint = (a + b) * 0.5;
			return center + (midpoint - center).normalize() * radius;
		}
	};
}

#endif // _mpj_geodesicdome_h_
