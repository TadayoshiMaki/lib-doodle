#include "doodle/HsvSensor.h"

#include "doodle/HsvSensorEval.h"
#include "doodle/HsvSensorProd.h"
#include "doodle/HsvSensorProdMultitrack.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

HsvSensor::Model HsvSensorFactory::getModel(const std::string& name)
{
    HsvSensor::Model model = HsvSensor::Model::kUnknownVendor_UnknowSensor;
    if (name == "Sony_EvaluationHsvSensor")
    {
        model = HsvSensor::Model::kSony_EvaluationHsvSensor;
    }
    else if (name == "OmronSentech_HsvChipSensor")
    {
        model = HsvSensor::Model::kOmronSentech_HsvChipSensor;
    }
    else if (name == "OmronSentech_HsvChipSensor_MultiTrack")
    {
        model = HsvSensor::Model::kOmronSentech_HsvChipSensor_MultiTrack;
    }
    return model;
}

std::shared_ptr<HsvSensor> doodle::HsvSensorFactory::create(const HsvSensor::Model model)
{
    std::shared_ptr<HsvSensor> sensor;
    switch (model)
    {
    case HsvSensor::Model::kSony_EvaluationHsvSensor:
        sensor = std::make_shared<SonyHsvSensor>();
        break;
    case HsvSensor::Model::kOmronSentech_HsvChipSensor:
        sensor = std::make_shared<SentechHsvSensor>();
        break;
    case HsvSensor::Model::kOmronSentech_HsvChipSensor_MultiTrack:
        sensor = std::make_shared<SentechHsvSensorMultiTrack>();
        break;
    default:
        break;
    }
    return std::move(sensor);
}

void doodle::HsvSensor::assignSensingState(std::vector<SensingResult>& curResults, const std::vector<SensingResult>& preResults, int64_t currentTimeMicros)
{
    //assigns detect state
    for (auto& cRes : curResults) {
        bool findSameId = false;
        for (auto& pRes : preResults) {
            if (cRes.trackingId == pRes.trackingId) {
                findSameId = true;
                if (pRes.sensingState == SensingState::LOST) {
                    //Lost -> Detet. assigns state detect.
                    cRes.sensingState = SensingState::DETECT;
                    break;
                }
            }
        }
        if (!findSameId) {
            //None -> Detect. assigns state detect.
            cRes.sensingState = SensingState::DETECT;
            continue;
        }
    }

    //assigns track and lost state
    for (auto& pRes : preResults) {
        bool findSameId = false;
        for (auto& cRes : curResults) {
            if (pRes.trackingId == cRes.trackingId &&
                (pRes.sensingState == SensingState::DETECT ||
                 pRes.sensingState == SensingState::TRACK))
            {
                //Detect -> Track or Track -> Track. assigns state track
                cRes.sensingState = SensingState::TRACK;
                findSameId = true;
            }
        }
        if (!findSameId && 
            (pRes.sensingState == SensingState::DETECT || 
             pRes.sensingState == SensingState::TRACK))
        {
            //Detect -> None or TRACK -> None. assigns state lost
            curResults.emplace_back(SensingState::LOST, static_cast<unsigned char>(pRes.trackingId),
                pRes.x,
                pRes.y,
                currentTimeMicros);
        }
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
