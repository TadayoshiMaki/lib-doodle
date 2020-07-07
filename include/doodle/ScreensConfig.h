#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreensConfig_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreensConfig_H_

#include <memory>

#include <type_traits>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/Types.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class Screen
{
public:
    Screen(const int id,
           std::vector<Point> contour,
           Angle mpjAngle,
           std::vector<PointF> refPoints,
           std::vector<PointF> points);
    virtual ~Screen();

    /**
     * @brief Returns the ID of a screen.
     */
    int getId() const
    {
        return id_;
    }

    /**
     * @brief Returns screen contour points.
     */
    std::vector<Point> getContour()
    {
        return contour_;
    }

    /**
     * @brief Returns a motor angle representing projection direction.
     */
    Angle getAngle()
    {
        return mpjAngle_;
    }

    /**
     * @brief Returns screen plane reference points.
     */
    std::vector<PointF> getPlaneReferencePoints() const
    {
        return refPoints_;
    }

    /**
     * @brief Returns screen plane points.
     */
    std::vector<PointF> getPlanePoints() const
    {
        return points_;
    }

    /**
     * @brief Determines whether the contour of a screen contains the coordinates or not.
     * @param p The coordinates.
     */
    bool contains(const Point p)
    {
        return contains(p.x, p.y);
    }

    /**
     * @brief Determines whether the contour of a screen contains the coordinates or not.
     * @param x The x-axis value.
     * @param y The y-axis value.
     */
    bool contains(const int x, const int y);

private:
    // The unique ID in a screen configuration.
    int id_;

    // The screen contour points on a fisheye lens camera view.
    std::vector<Point> contour_;
    // The motor angle representing projection direction.
    Angle mpjAngle_;

    // The screen reference points.
    std::vector<PointF> refPoints_;
    // The screen plane points on a camera view taking a projection area.
    std::vector<PointF> points_;
};


class ScreensConfig
{
public:
    ScreensConfig(const std::vector<std::shared_ptr<Screen>>& screens);
    virtual ~ScreensConfig();

    /**
     * @brief Returns all screens of a configuration.
     */
    std::vector<std::shared_ptr<Screen>> getScreens()
    {
        return screensEntries_;
    }

    /**
     * @brief Returns a screen of the specified index.
     */
    std::shared_ptr<Screen> getScreen(const int index)
    {
        if (index >= screensEntries_.size()) return nullptr;
        return screensEntries_.at(index);
    }

    size_t size()
    {
        return screensEntries_.size();
    }

    bool empty()
    {
        return screensEntries_.empty();
    }

private:
    // The all screens of this configuration.
    std::vector<std::shared_ptr<Screen>> screensEntries_;

};

class ScreensConfigLoader
{
public:
    /**
     * @brief Loads a screens configuration from the specified file.
     */
    virtual std::shared_ptr<ScreensConfig> load(const std::string& filePath) = 0;
};

class JsonScreensConfigLoader : public ScreensConfigLoader
{
public:
    JsonScreensConfigLoader();
    virtual ~JsonScreensConfigLoader();

    /**
     * @brief Loads a screens configuration from the specified file.
     */
    virtual std::shared_ptr<ScreensConfig> load(const std::string& configFilePath) override;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreensConfig_H_
