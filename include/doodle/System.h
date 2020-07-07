#ifndef LIB_DOODLE_FRAMEWORK_doodle_System_H_
#define LIB_DOODLE_FRAMEWORK_doodle_System_H_

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "mpj_camera.h"
#include "mpj_motor.h"
#include "mpj_string.h"

#include "picojson.h"

#include "doodle/namespace.h"
#include "doodle/HsvSensor.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using mpjsdk::mpj_KnownCameras;
using mpjsdk::mpj_KnownMotors;
using mpjsdk::mpj_StringUtils;
using doodle::HsvSensor;

/**
 * TODO: Should rename this class to "Context" or "SystemContext".
 */
class System
{
public:
    struct Resolution
    {
        uint32_t width;
        uint32_t height;
    };



    class MotorProperties
    {
    public:
        MotorProperties(const std::string& model,
                        const std::string& comPort,
                        const std::string& paramsFilePath) :
            model_(model),
            comPort_(comPort),
            paramsFilePath_(paramsFilePath)
        {
        }

        virtual ~MotorProperties()
        {
        }

        /**
         * @brief Returns a motor model name.
         * @see mpjsdk::mpj_KnownMotors
         */
        std::string getModel()
        {
            return model_;
        }

        /**
         * @brief Returns a COM port number.
         */
        std::string getComPort()
        {
            return comPort_;
        }

        /**
         * @brief Returns the path to a motor parameter file.
         */
        std::string getParameterFilePath()
        {
            return paramsFilePath_;
        }
    private:
        const std::string model_;
        const std::string comPort_;
        const std::string paramsFilePath_;
    };



    class FisheyeCameraProperties
    {
    public:
        FisheyeCameraProperties(const std::string& model,
                                const std::string& maskFilePath = std::string()) :
            model_(model), maskFilePath_(maskFilePath)
        {
        }

        virtual ~FisheyeCameraProperties()
        {
        }

        std::string getModel()
        {
            return model_;
        }

        std::string getMaskFilePath()
        {
            return maskFilePath_;
        }

    private:
        const std::string model_;
        // TODO: Create a parameter file.
        const std::string maskFilePath_;
    };



    class VisionSensorProperties
    {
    public:
        VisionSensorProperties(const std::string& modelName,
                               const std::string& setupFilePath,
                               const std::string& paramsDirName,
                               const std::string& calibFilePath) :
            calibFilePath_(calibFilePath)
        {
            using doodle::HsvSensorFactory;

            model_ = HsvSensorFactory::getModel(modelName);
            config_ = createConfiguration(model_,
                                          setupFilePath,
                                          paramsDirName);
        }

        virtual ~VisionSensorProperties()
        {
        }

        /**
         * @brief Returns a model name of a high-speed vision sensor.
         * @see doodle::HsvSensor::Model
         */
        HsvSensor::Model getModel()
        {
            return model_;
        }

        /**
         * @brief Returns a configuration of a high-speed vision sensor.
         */
        std::shared_ptr<HsvSensor::Config> getConfiguration()
        {
            return config_;
        }

        std::string getCalibrationFilePath()
        {
            return calibFilePath_;
        }
    private:
        HsvSensor::Model model_;
        std::shared_ptr<HsvSensor::Config> config_;
        const std::string calibFilePath_;

        std::shared_ptr<HsvSensor::Config> createConfiguration(const HsvSensor::Model model,
            const std::string& setupFilePath,
            const std::string& paramsDirName);
    };



    class ProjectorProperties
    {
    public:
        ProjectorProperties(const std::string model,
                            const int brightness,
                            const Resolution nativeResolution,
                            const Resolution resolution,
                            int frequency) :
            model_(model),
            brightness_(brightness),
            nativeResolution_(nativeResolution),
            resolution_(resolution),
            frequency_(frequency)
        {
        }

        virtual ~ProjectorProperties()
        {
        }

        std::string getModel()
        {
            return model_;
        }

        int getBrightness()
        {
            return brightness_;
        }

        Resolution getNativeResolution()
        {
            return nativeResolution_;
        }

        Resolution getResolution()
        {
            return resolution_;
        }

        int getFrequency()
        {
            return frequency_;
        }

    private:
        const std::string model_;
        const int brightness_;
        const Resolution nativeResolution_;
        const Resolution resolution_;
        const int frequency_;
    };



    class ScreenLayoutProperties
    {
    public:
        ScreenLayoutProperties(const std::string& filePath) :
            filePath_(filePath)
        {
        }

        virtual ~ScreenLayoutProperties()
        {
        }

        std::string getFilePath()
        {
            return filePath_;
        }
    private:
        const std::string filePath_;
    };

    System()
    {
    }

    virtual ~System()
    {
    }

    /**
     * @brief Loads system properties from the specified file.
     * @param file The relative file path.
     * @return true if successfully loaded.
     */
    bool loadProperties(const std::string& propertiesFilePath)
    {
        _FSPFX path filePath(propertiesFilePath);
        if (!_FSPFX exists(filePath)) return false;

        std::ifstream ifs(propertiesFilePath);
        std::string buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        picojson::value systemProperties;
        auto error = picojson::parse(systemProperties, buffer.begin(), buffer.end());
        if (!error.empty())
        {
            // JSON deserialization error.
            return false;
        }

        motor_ = loadMotorProperties(systemProperties);
        fisheyeCamera_ = loadFisheyeCameraProperties(systemProperties);
        visionSensor_ = loadVisionSensorProperties(systemProperties);
        projector_ = loadProjectorProperties(systemProperties);
        layout_ = loadScreenLayoutProperties(systemProperties);
        return true;
    }

    std::shared_ptr<MotorProperties> getMotorProperties()
    {
        return motor_;
    }

    std::shared_ptr<FisheyeCameraProperties> getFisheyeCameraProperties()
    {
        return fisheyeCamera_;
    }

    std::shared_ptr<VisionSensorProperties> getVisionSensorProperties()
    {
        return visionSensor_;
    }

    std::shared_ptr<ProjectorProperties> getProjectorProperties()
    {
        return projector_;
    }

    std::shared_ptr<ScreenLayoutProperties> getScreenLayoutProperties()
    {
        return layout_;
    }

private:
    std::shared_ptr<MotorProperties> motor_;
    std::shared_ptr<FisheyeCameraProperties> fisheyeCamera_;
    std::shared_ptr<VisionSensorProperties> visionSensor_;
    std::shared_ptr<ProjectorProperties> projector_;
    std::shared_ptr<ScreenLayoutProperties> layout_;

    std::shared_ptr<MotorProperties> loadMotorProperties(const picojson::value& systemProperties)
    {
        if (!systemProperties.contains("motor")) return nullptr;

        auto& value = systemProperties.get("motor");
        if (!value.is<picojson::object>()) return nullptr;

        auto& obj = value.get<picojson::object>();
        std::string model = obj.at("model").get<std::string>();
        std::string comPort = obj.at("com_port").get<std::string>();
        std::string paramsFilename = obj.at("params_file").get<std::string>();
        auto properties = std::make_shared<MotorProperties>(model, comPort, paramsFilename);
        return std::move(properties);
    }

    std::shared_ptr<FisheyeCameraProperties> loadFisheyeCameraProperties(const picojson::value& systemProperties)
    {
        if (!systemProperties.contains("fisheye_camera")) return nullptr;

        auto& value = systemProperties.get("fisheye_camera");
        if (!value.is<picojson::object>()) return nullptr;

        auto& obj = value.get<picojson::object>();
        std::string model = obj.at("model").get<std::string>();

        picojson::object::const_iterator it = obj.find("mask_file");
        std::string maskFilePath;
        if (it != obj.cend())
        {
            maskFilePath = it->second.get<std::string>();
        }

        auto properties = std::make_shared<FisheyeCameraProperties>(model, maskFilePath);
        return std::move(properties);
    }

    std::shared_ptr<VisionSensorProperties> loadVisionSensorProperties(const picojson::value& systemProperties)
    {
        if (!systemProperties.contains("vision_sensor")) return nullptr;

        auto& value = systemProperties.get("vision_sensor");
        auto& obj = value.get<picojson::object>();
        auto model = obj.at("model").get<std::string>();
        auto setupFilePath = obj.at("setup_file").get<std::string>();
        std::string paramsDirPath;
        picojson::object::const_iterator dirIt = obj.find("params_directory");
        if (dirIt != obj.cend())
        {
            paramsDirPath = obj.at("params_directory").get<std::string>();
        }

        picojson::object::const_iterator calibIt = obj.find("calibration_file");
        std::string calibFilePath;
        if (calibIt != obj.cend())
        {
            calibFilePath = obj.at("calibration_file").get<std::string>();
        }
        auto properties = std::make_shared<VisionSensorProperties>(model,
                                                                   setupFilePath,
                                                                   paramsDirPath,
                                                                   calibFilePath);
        return std::move(properties);
    }

    std::shared_ptr<ProjectorProperties> loadProjectorProperties(const picojson::value& systemProperties)
    {
        auto& value = systemProperties.get("projector");
        auto& obj = value.get<picojson::object>();
        std::string model = obj.at("model").get<std::string>();
        int brightness = static_cast<int>(obj.at("brightness").get<double>());
        auto& nativeResolutionObj = obj.at("native_resolution").get<picojson::object>();
        int nativeResolutionWidth = static_cast<int>(nativeResolutionObj.at("width").get<double>());
        int nativeResolutionHeight = static_cast<int>(nativeResolutionObj.at("height").get<double>());
        Resolution nativeResolution{ nativeResolutionWidth, nativeResolutionHeight };
        auto& resolutionObj = obj.at("resolution").get<picojson::object>();
        int resolutionWidth = static_cast<int>(resolutionObj.at("width").get<double>());
        int resolutionHeight = static_cast<int>(resolutionObj.at("height").get<double>());
        Resolution resolution{ resolutionWidth, resolutionHeight };
        int frequency = static_cast<int>(obj.at("frequency").get<double>());
        auto properties = std::make_shared<ProjectorProperties>(model,
                                                                brightness,
                                                                nativeResolution,
                                                                resolution,
                                                                frequency);
        return std::move(properties);
    }

    std::shared_ptr<ScreenLayoutProperties> loadScreenLayoutProperties(const picojson::value& systemProperties)
    {
        auto& value = systemProperties.get("screen_layout");
        auto& obj = value.get<picojson::object>();
        std::string model = obj.at("layout_file").get<std::string>();
        auto properties = std::make_shared<ScreenLayoutProperties>(model);
        return std::move(properties);
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_System_H_
