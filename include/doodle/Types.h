#ifndef LIB_DOODLE_FRAMEWORK_doodle_Types_H_
#define LIB_DOODLE_FRAMEWORK_doodle_Types_H_

#include "mpj_angle.h"
#include "opencv2/core/types.hpp"

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using mpjsdk::mpj_Angle;

using Point = cv::Point;
using PointF = cv::Point2d;

using Size = cv::Size;

using Angle = mpj_Angle;

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_Types_H_
