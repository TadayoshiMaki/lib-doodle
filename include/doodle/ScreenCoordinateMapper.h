#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenCoordinateMapper_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenCoordinateMapper_H_

#include <memory>
#include <vector>

#include "opencv2/core/types.hpp"

#include "doodle/namespace.h"
#include "doodle/ScreensConfig.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenCoordinateMapper
{
public:
    ScreenCoordinateMapper(const std::vector<PointF>& points,
                           const std::vector<PointF>& refPoints);
    virtual ~ScreenCoordinateMapper();

    /**
     * @brief Returns the matrix of this mapper.
     */
    std::shared_ptr<cv::Mat> getMatrix();

    /**
     * @brief Maps the specified coordinates to coordinates on the screen.
     * @param x The x-axis value.
     * @param y The y-axis value.
     */
    std::shared_ptr<PointF> map(const float x, const float y);
    /**
     * @brief Maps the specified coordinates to coordinates on the screen.
     * @param point The point.
     */
    std::shared_ptr<PointF> map(PointF point);

private:
    // The homography matrix to map coordinates to a screen.
    std::shared_ptr<cv::Mat> h_;

    /**
     * @brief Maps the specified coordinates to coordinates on the screen.
     * @param point The points.
     */
    std::vector<PointF> map(const std::vector<PointF>& srcPoints);
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenCoordinateMapper_H_
