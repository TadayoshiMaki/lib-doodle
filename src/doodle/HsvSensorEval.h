#ifndef LIB_DOODLE_FRAMEWORK_doodle_HsvSensorEval_H_
#define LIB_DOODLE_FRAMEWORK_doodle_HsvSensorEval_H_

#include <memory>
#include <string>

#include "HsvCam.h"

#include "doodle/namespace.h"
#include "doodle/HsvSensor.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using so::HsvCam;
using so::SensingInformationPacket_t;

class SonyHsvSensor : public HsvSensor
{

public:
    class Config : public HsvSensor::Config
    {
    public:
        Config(const std::string& setupFilePath, const std::string& paramsSubDir) :
            setupFilePath_(setupFilePath), paramsSubDir_(paramsSubDir)
        {
        }

        ~Config()
        {
        }

        std::string getSetupFilePath()
        {
            return setupFilePath_;
        }

        std::string getParamsSubDirectory()
        {
            return paramsSubDir_;
        }

    private:
        std::string setupFilePath_;
        std::string paramsSubDir_;
    };

    SonyHsvSensor();
    virtual ~SonyHsvSensor();

    /**
     * @brief Returns the width of captured frames.
     * @return The width in pixels.
     */
    uint32_t getWidth() override;

    /**
     * @brief Returns the height of captured frames.
     * @return The height in pixels.
     */
    uint32_t getHeight() override;


    /**
     * @brief Returns the pixel format of captured frames.
     * @return The pixel format.
     */
    HsvSensor::PixelFormat getPixelFormat() override;

protected:
    /**
     * @brief Called to open a high-speed vision sensor.
     * @return true if successfully opened.
     */
    bool onOpenSensor(const std::shared_ptr<HsvSensor::Config>& config) override;

    /**
     * @brief Called to close a high-speed vision sensor.
     */
    void onCloseSensor() override;
    /**
     * @brief Called to start a high-speed vision sensor.
     * @return true if successfully started.
     */
    bool onStartSensor() override;
    /**
     * @brief Called to stop a high-speed vision sensor.
     * @return true if successfully stopped.
     */
    bool onStopSensor() override;
    /**
     * @brief Captures frames in a loop on the worker thread.
     */
    void run() override;

private:
    // The high speed vision sensor instance.
    std::shared_ptr<HsvCam> camera_;
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_HsvSensorEval_H_
