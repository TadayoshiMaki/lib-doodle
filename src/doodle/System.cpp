#include "doodle/HsvSensor.h"
#include "doodle/System.h"

#include "doodle/HsvSensorEval.h"
#include "doodle/HsvSensorProd.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

std::shared_ptr<HsvSensor::Config>
        System::VisionSensorProperties::createConfiguration(
    const HsvSensor::Model model,
    const std::string& setupFilePath,
    const std::string& paramsDirName)
{
    using doodle::SonyHsvSensor;
    using doodle::SentechHsvSensor;

    switch (model)
    {
    case HsvSensor::Model::kSony_EvaluationHsvSensor:
        return std::make_shared<SonyHsvSensor::Config>(setupFilePath,
            paramsDirName);
    case HsvSensor::Model::kOmronSentech_HsvChipSensor:
    case HsvSensor::Model::kOmronSentech_HsvChipSensor_MultiTrack:
        return std::make_shared<SentechHsvSensor::Config>(setupFilePath);
    default:
        return nullptr;
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
