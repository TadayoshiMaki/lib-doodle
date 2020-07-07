#include "doodle/ScreenCoordinateMapper.h"

#include "opencv2/calib3d.hpp"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

ScreenCoordinateMapper::ScreenCoordinateMapper(const std::vector<PointF>& points,
                                               const std::vector<PointF>& refPoints)
{
    auto h = cv::findHomography(points, refPoints);
    h_ = std::make_shared<cv::Mat>(std::move(h));
}

ScreenCoordinateMapper::~ScreenCoordinateMapper()
{
}

std::shared_ptr<cv::Mat> ScreenCoordinateMapper::getMatrix()
{
    return h_;
}

std::shared_ptr<PointF> ScreenCoordinateMapper::map(const float x, const float y)
{
    std::vector<PointF> srcPoints;
    srcPoints.emplace_back(x, y);
    std::vector<PointF> dstPoints = map(srcPoints);
    return std::make_shared<PointF>(std::move(dstPoints.at(0)));
}

std::shared_ptr<PointF> ScreenCoordinateMapper::map(PointF point)
{
    std::vector<PointF> srcPoints;
    srcPoints.push_back(point);
    std::vector<PointF> dstPoints = map(srcPoints);
    return std::make_shared<PointF>(std::move(dstPoints.at(0)));
}

std::vector<PointF> ScreenCoordinateMapper::map(const std::vector<PointF>& srcPoints)
{
    std::vector<PointF> dstPoints;
    cv::perspectiveTransform(srcPoints, dstPoints, *h_);
    return std::move(dstPoints);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
