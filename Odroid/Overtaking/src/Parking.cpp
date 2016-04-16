//
// Created by marco on 2016-04-07.
//

#include "Parking.h"

//
// Created by niklas on 2016-04-05.
//

#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

#include "Parking.h"

using namespace std;
using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace over::take;

Parking::Parking(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Parking") {}

Parking::~Parking() {}

void Parking::setUp(){
    cout << "Parking starts" << endl;
}
void Parking::tearDown(){
    cout << "This is when Parking stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parking::body() {

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        // 1. Get most recent vehicle data:
        Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        VehicleData vd = containerVehicleData.getData<VehicleData> ();

        // 2. Get most recent sensor board data describing virtual sensor data:
        Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

        // Create vehicle control data.
        VehicleControl vc;

        cout << "Parking: "  << vd.getSpeed() << endl;

        cout << "This is the amount of sensors: " << sbd.getNumberOfSensors() << endl;

        // Create container for finally sending the data.
        //Container c(Container::VEHICLECONTROL, vc);
        // Send container.
        //getConference().send(c);
        cout << "This is printed" << endl;

    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}