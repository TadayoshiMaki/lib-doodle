#include "doodle/CameraUtils.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "mpj_string.h"
#include "picojson.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

CameraParameters::CameraParameters(std::vector<double> cameraMatrix,
                                   std::vector<double> distCoeffs,
                                   Size imageSize) :
    cameraMatrix_(cameraMatrix),
    distCoeffs_(distCoeffs),
    imageSize_(imageSize)
{
}

CameraParameters::~CameraParameters()
{
}



Undistorter::Undistorter(const std::shared_ptr<CameraParameters>& cameraParams) :
    cameraParams_(cameraParams)
{
    newCameraMatrix_ = cv::getOptimalNewCameraMatrix(
        cameraParams_->getCameraMatrix(),
        cameraParams_->getDistortionCoefficients(),
        cameraParams_->getImageSize(),
        1.0,
        cameraParams_->getImageSize());
}

Undistorter::~Undistorter()
{
}

std::shared_ptr<PointF> Undistorter::undistort(const float x, const float y)
{
    std::vector<PointF> srcPoints;
    srcPoints.emplace_back(x, y);
    std::vector<PointF> dstPoints = undistort(srcPoints);
    return std::make_shared<PointF>(std::move(dstPoints.at(0)));
}

std::shared_ptr<PointF> Undistorter::undistort(PointF point)
{
    std::vector<PointF> srcPoints;
    srcPoints.push_back(point);
    std::vector<PointF> dstPoints = undistort(srcPoints);
    return std::make_shared<PointF>(std::move(dstPoints.at(0)));
}

std::vector<PointF> Undistorter::undistort(const std::vector<PointF>& srcPoints)
{
    std::vector<PointF> dstPoints;
    cv::undistortPoints(srcPoints,
                        dstPoints,
                        cameraParams_->getCameraMatrix(),
                        cameraParams_->getDistortionCoefficients(),
                        cv::Mat(),
                        newCameraMatrix_);

    for (auto&& dstPoint : dstPoints)
    {
        ensureValidCoordinates(&dstPoint);
    }
    return std::move(dstPoints);
}

void Undistorter::ensureValidCoordinates(PointF* point)
{
    const auto& maxSize = cameraParams_->getImageSize();
    if (point->x < 0)
    {
        point->x = 0;
    }
    else if (point->x > maxSize.width)
    {
        point->x = maxSize.width;
    }

    if (point->y < 0)
    {
        point->y = 0;
    }
    else if (point->y > maxSize.height)
    {
        point->y = maxSize.height;
    }
}



JsonCameraParamsLoader::JsonCameraParamsLoader()
{
}

JsonCameraParamsLoader::~JsonCameraParamsLoader()
{
}

std::shared_ptr<CameraParameters> JsonCameraParamsLoader::load(const std::string& calibFilePath)
{
    using mpjsdk::mpj_StringUtils;

    _FSPFX path filePath(calibFilePath);
    if (!_FSPFX exists(filePath)) return nullptr;

    std::ifstream ifs(filePath);
    std::string buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    picojson::value cameraParamsData;
    auto error = picojson::parse(cameraParamsData, buffer.begin(), buffer.end());
    if (!error.empty())
    {
        // JSON deserialization error.
        return nullptr;
    }

    auto& imageSizeValue = cameraParamsData.get("image_size");
    auto& imageSizeObj = imageSizeValue.get<picojson::object>();
    int width = static_cast<int>(imageSizeObj.at("width").get<double>());
    int height = static_cast<int>(imageSizeObj.at("height").get<double>());
    Size imageSize(width, height);

    std::vector<double> cameraMatrix;
    auto matrixValue = cameraParamsData.get("camera_matrix");
    auto matrixValuesArray = matrixValue.get<picojson::array>();
    auto matrixValuesArraySize = matrixValuesArray.size();
    for (size_t i = 0; i < matrixValuesArraySize; i++)
    {
        cameraMatrix.push_back(matrixValuesArray[i].get<double>());
    }

    std::vector<double> distCoeffs;
    auto distCoeffsValue = cameraParamsData.get("distortion_coefficients");
    auto distCoeffsArray = distCoeffsValue.get<picojson::array>();
    auto distCoeffsArraySize = distCoeffsArray.size();
    for (size_t i = 0; i < distCoeffsArraySize; i++)
    {
        distCoeffs.push_back(distCoeffsArray[i].get<double>());
    }

    auto params = std::make_shared<CameraParameters>(cameraMatrix, distCoeffs, imageSize);
    return std::move(params);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
