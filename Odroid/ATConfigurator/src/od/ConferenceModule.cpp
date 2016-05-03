//
// Created by jonas on 4/28/16.
//

#include "od/ConferenceModule.h"

#include "od/ConferenceData.h"
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>

using namespace std;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

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

        //Get the current xyz
        Container sbc = getKeyValueDataStore().get(SensorBoardData::ID());
        SensorBoardData sensorBoardData = sbc.getData<SensorBoardData>();
        map<uint32_t, double> sensorMap = sensorBoardData.getMapOfDistances();
        ConferenceData::instance()->setUS1(sensorMap[3]);
        ConferenceData::instance()->setUS2(sensorMap[4]);
        ConferenceData::instance()->setIR1(sensorMap[0]);
        ConferenceData::instance()->setIR2(sensorMap[2]);
        ConferenceData::instance()->setIR3(sensorMap[1]);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
