#include "doodle/ScreensConfig.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "mpj_string.h"
#include "picojson.h"
#include "opencv2/imgproc.hpp"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

Screen::Screen(const int id,
               std::vector<Point> contour,
               Angle mpjAngle,
               std::vector<PointF> refPoints,
               std::vector<PointF> points) :
    id_(id),
    contour_(contour),
    mpjAngle_(mpjAngle),
    refPoints_(refPoints),
    points_(points)
{
}

Screen::~Screen()
{
}

bool Screen::contains(const int x, const int y)
{
    // Note that an input contour array should be an integer array.
    return (cv::pointPolygonTest(contour_, Point(x, y), false) >= 0);
}



ScreensConfig::ScreensConfig(const std::vector<std::shared_ptr<Screen>>& screens) :
    screensEntries_(screens)
{
}


ScreensConfig::~ScreensConfig()
{
}

JsonScreensConfigLoader::JsonScreensConfigLoader()
{
}

JsonScreensConfigLoader::~JsonScreensConfigLoader()
{
}

std::shared_ptr<ScreensConfig> JsonScreensConfigLoader::load(const std::string& configFilePath)
{
    using mpjsdk::mpj_StringUtils;

    _FSPFX path filePath(configFilePath);
    if (!_FSPFX exists(filePath)) return nullptr;

    std::ifstream ifs(configFilePath);
    std::string buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    picojson::value screensData;
    auto error = picojson::parse(screensData, buffer.begin(), buffer.end());
    if (!error.empty())
    {
        // JSON deserialization error.
        return nullptr;
    }

    auto& value = screensData.get("screens");
    if (!value.is<picojson::array>()) return nullptr;
    auto& screensArray = value.get<picojson::array>();

    std::vector<picojson::object> screenObjects;
    auto screensArraySize = screensArray.size();
    for (size_t i = 0; i < screensArraySize; i++)
    {
        if (!screensArray[i].is<picojson::object>()) return nullptr;
        screenObjects.push_back(screensArray[i].get<picojson::object>());
    }

    std::vector<std::shared_ptr<Screen>> screens;
    for (auto&& screenObject : screenObjects)
    {
        int id = static_cast<int>(screenObject["id"].get<double>());

        std::vector<Point> contour;
        if (screenObject.find("contour") != screenObject.cend())
        {
            auto& contourObject = screenObject["contour"].get<picojson::object>();
            auto& contourCoordinatesArray = contourObject["coordinates"].get<picojson::array>();
            auto contourCoordinatesSize = contourCoordinatesArray.size();
            for (int i = 0; i < contourCoordinatesSize; i++)
            {
                if (!contourCoordinatesArray[i].is<picojson::object>()) continue;
                auto& coordinatesObject = contourCoordinatesArray[i].get<picojson::object>();
                int x = static_cast<int>(coordinatesObject["x"].get<double>());
                int y = static_cast<int>(coordinatesObject["y"].get<double>());
                contour.emplace_back(x, y);
            }
        }

        Angle angle;
        if (screenObject.find("motor") != screenObject.cend())
        {
            auto& motorObject = screenObject["motor"].get<picojson::object>();
            float panDegrees = static_cast<float>(motorObject["pan_degrees"].get<double>());
            float tiltDegrees = static_cast<float>(motorObject["tilt_degrees"].get<double>());
            angle.pan = panDegrees;
            angle.tilt = tiltDegrees;
        }

        std::vector<PointF> refPoints;
        std::vector<PointF> points;
        if (screenObject.find("plane") != screenObject.cend())
        {
            auto& planeObject = screenObject["plane"].get<picojson::object>();
            auto& planeCoordinatesArray = planeObject["coordinates"].get<picojson::array>();
            auto planeCoordinatesSize = planeCoordinatesArray.size();
            for (int i = 0; i < planeCoordinatesSize; i++)
            {
                if (!planeCoordinatesArray[i].is<picojson::object>()) continue;
                auto& coordinatesObject = planeCoordinatesArray[i].get<picojson::object>();
                int refX = static_cast<int>(coordinatesObject["reference_x"].get<double>());
                int refY = static_cast<int>(coordinatesObject["reference_y"].get<double>());
                refPoints.emplace_back(refX, refY);
                int x = static_cast<int>(coordinatesObject["x"].get<double>());
                int y = static_cast<int>(coordinatesObject["y"].get<double>());
                points.emplace_back(x, y);
            }
        }

        auto screen = std::make_shared<Screen>(id, contour, angle, refPoints, points);
        screens.push_back(std::move(screen));
    }

    auto screensConfig = std::make_shared<ScreensConfig>(std::move(screens));
    return std::move(screensConfig);
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
