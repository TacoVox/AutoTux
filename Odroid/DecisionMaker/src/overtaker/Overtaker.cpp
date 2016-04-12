//
// Created by marco on 2016-04-07.
//
#include <iostream>

#include "overtaker/Overtaker.h"

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

//using namespace odcore::base;
//using namespace odcore::base::module;
using namespace odcore::data;               // Allows 'Container'
using namespace automotive::miniature;      // Allows 'Sensor Board Data'
using namespace automotive;                 // Allows 'Vehicle Data'

using namespace overtaker;

/* @doc Constructor.
 * */
Overtaker::Overtaker(const int32_t &argc, char **argv):
    TimeTriggeredConferenceClientModule(argc, argv, "Overtaker"),
    ovtControlDataContainer(NULL) {}

Overtaker::~Overtaker(){}

void Overtaker::setUp() {
    cout << "Starting Overtaker component..." << endl;
}

void Overtaker::tearDown() {
    cout << "Terminating Overtaker component..." << endl;
}

/* @doc Setter for private Overtaker control data container
 * */
void Overtaker::setOvtControlDataContainer(std::shared_ptr<odcore::data::Container> container){
    this->ovtControlDataContainer = container;
}

/* @doc Returns true 'sensor' detects an object within maxDistance
 * */
bool Overtaker::isObstacleDetected(automotive::miniature::SensorBoardData sbd, const double sensor, const double maxDist) {

    cout << "Front US sensor : " << sbd.getValueForKey_MapOfDistances(sensor) << endl;        // DEBUG** Remove this line

    if(sbd.getValueForKey_MapOfDistances(sensor) < maxDist) return true;

    return false;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Overtaker::body(){

    const double ULTRASONIC_FRONT_RIGHT = 4;
    const double ULTRASONIC_FRONT_FORWARD = 3;
    const double INFRARED_FRONT_RIGHT = 0;
    const double INFRARED_REAR_RIGHT = 2;
    const double INFRARED_REAR_BACK = 1;

    // Overtaker sub-states
    enum STATE {FREE_LANE, APPROACHING, LEFT_SWITCH, PARALLEL, RIGHT_SWITCH};

    STATE state = FREE_LANE;

    Container containerVehicleData;
    VehicleData vd;
    Container containerSensorBoardData;
    SensorBoardData sbd;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING){

        //cout << "Overtaker loop" << endl;

        // 1. Get most recent vehicle data:
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        // 2. Get most recent sensor board data describing virtual sensor data:
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        switch(state){
            case FREE_LANE:{

                // Check front for obstacles
                if(isObstacleDetected(sbd, ULTRASONIC_FRONT_FORWARD, 30.0)){
                    cout << "Overtaker: Obstacle detected" << endl;
                    //state = APPROACHING;
                }
            }
            case APPROACHING:{
                // To-Do...
                    // Measure distance from obstacle
            }
        }

        /*
        if(state == FREE_LANE){

            cout << "Overtaker state: FREE_LANE" << endl;

            // Check front for obstacles
            if(isObstacleDetected(sbd, ULTRASONIC_FRONT_FORWARD, 30.0)){
                cout << "Obstacle detected, switching to approaching obstacle" << endl;
                //state = APPROACHING;
            }
        }else if(state == APPROACHING){
            //To-Do...
            // Measure Distance from obstacle and compare with threshold
        }

         */
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
