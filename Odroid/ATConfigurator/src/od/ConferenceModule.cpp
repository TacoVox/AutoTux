//
// Created by Jonas Kahler on 4/28/16.
//

#include "od/ConferenceModule.h"

#include "od/ConferenceData.h"
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>
#include <automotivedata/generated/autotux/config/LaneFollowerMSG.h>
#include <automotivedata/generated/autotux/LaneRecommendationMSG.h>
#include <automotivedata/generated/autotux/DecisionMakerMSG.h>

using namespace std;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;
using namespace autotux;
using namespace autotux::config;

od::ConferenceModule::ConferenceModule(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Configurator") {}

od::ConferenceModule::~ConferenceModule() {}

void od::ConferenceModule::setUp() {}

void od::ConferenceModule::tearDown() {}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode od::ConferenceModule::body() {
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
            odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        //Get the current VehicleControl data and put them in the storage
        Container vcc = getKeyValueDataStore().get(VehicleControl::ID());
        VehicleControl vehicleControl = vcc.getData<VehicleControl>();
        ConferenceData::instance()->setSpeed(vehicleControl.getSpeed());
        ConferenceData::instance()->setAngle(vehicleControl.getSteeringWheelAngle());

        //Get the current sensor board data
        Container sbc = getKeyValueDataStore().get(SensorBoardData::ID());
        SensorBoardData sensorBoardData = sbc.getData<SensorBoardData>();
        map<uint32_t, double> sensorMap = sensorBoardData.getMapOfDistances();
        ConferenceData::instance()->setUS1(sensorMap[3]);
        ConferenceData::instance()->setUS2(sensorMap[4]);
        ConferenceData::instance()->setIR1(sensorMap[0]);
        ConferenceData::instance()->setIR2(sensorMap[2]);
        ConferenceData::instance()->setIR3(sensorMap[1]);

        //Get the current LaneRecommendation
        Container lrc = getKeyValueDataStore().get(LaneRecommendationMSG::ID());
        LaneRecommendationMSG laneRecommendationMSG = lrc.getData<LaneRecommendationMSG>();
        ConferenceData::instance()->setRec_angle(laneRecommendationMSG.getAngle());
        ConferenceData::instance()->setDistance_to_line(laneRecommendationMSG.getDistance_to_line());
        ConferenceData::instance()->setQuality(laneRecommendationMSG.getQuality());

        //Send out the configured vals
        getConference().send(*od::ConferenceData::instance()->genLaneFollowerContainer());

        //Send state
        DecisionMakerMSG decisionMakerMSG;
        decisionMakerMSG.setState(ConferenceData::instance()->getState());
        Container dmc(decisionMakerMSG);
        getConference().send(dmc);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
