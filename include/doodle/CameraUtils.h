#ifndef LIB_DOODLE_FRAMEWORK_doodle_CameraUtils_H_
#define LIB_DOODLE_FRAMEWORK_doodle_CameraUtils_H_

#include <memory>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"

#include "doodle/namespace.h"
#include "doodle/Types.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class CameraParameters
{
public:
    CameraParameters(std::vector<double> cameraMatrix,
                     std::vector<double> distCoeffs,
                     Size imageSize);
    virtual ~CameraParameters();

    /**
     * @brief Returns a camera matrix.
     */
    cv::Mat getCameraMatrix()
    {
        // TODO: Should use 3x4 matrix.
        cv::Mat matrix(3, 3, CV_64FC1, cameraMatrix_.data());
        return std::move(matrix);
    }

    /**
     * @brief Returns distortion coefficients.
     */
    cv::Mat getDistortionCoefficients()
    {
        cv::Mat coeffs(distCoeffs_);
        return std::move(coeffs);
    }

    /**
     * @brief Returns an image size.
     */
    Size getImageSize()
    {
        return imageSize_;
    }

private:
    // The camera matrix, or intrinsic parameters.
    std::vector<double> cameraMatrix_;
    // The distortion coefficients.
    std::vector<double> distCoeffs_;
    // The image size.
    Size imageSize_;

};

class Undistorter
{
public:
    Undistorter(const std::shared_ptr<CameraParameters>& cameraParams);
    virtual ~Undistorter();

    /**
     * @brief Undistorts the specified coordinates.
     * @param x The x-axis value.
     * @param y The y-axis value.
     */
    std::shared_ptr<PointF> undistort(const float x, const float y);
    /**
     * @brief Undistorts the specified coordinates.
     * @param point The point.
     */
    std::shared_ptr<PointF> undistort(PointF point);

private:
    // The camera parameters used to undistort.
    std::shared_ptr<CameraParameters> cameraParams_;
    // The optimal new camera matrix.
    cv::Mat newCameraMatrix_;

    /**
     * @brief Undistorts the specified coordinates.
     * @param point The point.
     */
    std::vector<PointF> undistort(const std::vector<PointF>& srcPoints);

    /**
     * @brief Ensures that the coordinate are valid for an image size.
     * @param point The coordinates.
     */
    void ensureValidCoordinates(PointF* point);
};

class CameraParamsLoader
{
public:
    /**
     * @brief Loads a camera calibration parameters from the specified file.
     */
    virtual std::shared_ptr<CameraParameters> load(const std::string& calibFilePath) = 0;
};

class JsonCameraParamsLoader : public CameraParamsLoader
{
public:
    JsonCameraParamsLoader();
    virtual ~JsonCameraParamsLoader();

    /**
     * @brief Loads a camera calibration parameters from the specified file.
     */
    virtual std::shared_ptr<CameraParameters> load(const std::string& calibFilePath) override;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_CameraUtils_H_
