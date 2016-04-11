//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "parker/Parker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace parker;

Parker::Parker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Parker") {}

Parker::~Parker() {}

void Parker::setUp(){
    cout << "Parker starts" << endl;
}
void Parker::tearDown(){
    cout << "This is when Parker stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parker::body() {

    const double ULTRASONIC_FRONT_RIGHT = 4;
    const double ULTRASONIC_FRONT_FORWARD = 3;
    const double INFRARED_FRONT_RIGHT = 0;
    const double INFRARED_REAR_RIGHT = 2;
    const double INFRARED_REAR_BACK = 1;

    double ufr;
    double uff;
    double irfr;
    double irrr;
    double irrb;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        // 1. Get most recent vehicle data:
        Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        VehicleData vd = containerVehicleData.getData<VehicleData> ();

        // 2. Get most recent sensor board data describing virtual sensor data:
        Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

        //distance = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT);
        ufr = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT);
        uff = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);
        irfr = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
        irrr = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);
        irrb = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK);
/*
        cout << "Right UltraSonic: " << ufr << endl;
        cout << "Front UltraSonic: " << uff << endl;
        cout << "Front right IRSENSOR: " << irfr << endl;
        cout << "Rear right IRSENSOR: " << irrr << endl;
        cout << "Rear Back IRSENSOR: " << irrb << endl;
*/
        cout << "Abs traveled: " << vd.getAbsTraveledPath() << endl;
        cout << "REL traveled path: " << vd.getRelTraveledPath() << endl;
        cout << "Get heading: " << vd.getHeading() << endl;

        // Create vehicle control data.
        VehicleControl vc;

        if(parking){
            cout << "Now Parking" << endl;
            vc.setSpeed(2);
            vc.setSteeringWheelAngle(25);
            *parkingControler = vc;
        }
        cout << "Stopped parking" << endl;

    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
/**
 * This method sets the bool parking passed from DecisionMaker
 */
void Parker::setParking(std::shared_ptr<bool> parking){
    this->parking = parking;
}
void Parker::setParkingControler(std::shared_ptr<Container> parkingControler){
    this->parkingControler = parkingControler;
}


