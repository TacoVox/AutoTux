//
// Created by niklas on 2016-04-08.
//

#include <iostream>


#include "decisionmaker/DecisionMaker.h"
#include "overtaker/Overtaker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;               // Container
using namespace automotive;                 // Vehicle Control & Vehicle Data
using namespace automotive::miniature;      // Sensor Board Data

using namespace autotux;

using namespace decisionmaker;
using namespace overtaker;

enum STATE {DRIVING, PARKING};

// takes the values of the argument passed to the constructor
int32_t ptrargc;
char** ptrargv;

// Shared pointer to Container
VehicleControl vehicleControl;

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker"),
        laneRecommendation() , ovt(){
    ptrargc = argc;
    ptrargv = argv;
}

DecisionMaker::~DecisionMaker() {}

void DecisionMaker::setUp(){
    cout << "DecisionMaker is setting up" << endl;
}
void DecisionMaker::tearDown(){
    cout << "DecisionMaker shuts down" << endl;
}
/**
 * Sets wheelangledata to the LaneRecommandation
*/
void DecisionMaker::laneFollowing() {
    vehicleControl.setSteeringWheelAngle(getAngle());
}

/**
 * Gets the angle given by LaneRecommandation
 */
double DecisionMaker::getAngle() {
    return laneRecommendation.getData<LaneRecommendation>().getAngle();
}
/**
 * Gets a bool if the car is in leftlane
 * @TODO This isn't working yet
 */
bool DecisionMaker::isInLeftLane() {
    return laneRecommendation.getData<LaneRecommendation>().getLeft_lane();

}
/**
 * True if Data sent by the LaneRecommendation are considered reliable.
 */
bool DecisionMaker::isDataQuality(){
    return laneRecommendation.getData<LaneRecommendation>().getQuality();

}
/**
 * Get's the distance from a stop Line
 * @TODO is not found yet
 */
double DecisionMaker::getDistanceToLine() {
    return laneRecommendation.getData<LaneRecommendation>().getDistance_to_line();

}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DecisionMaker::body() {

    // Set initial state of the car
    STATE state = DRIVING;

    VehicleData vd;
    SensorBoardData sbd;

    // Set initial speed
    vehicleControl.setSpeed(2);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        // 1. Update vehicle data values
        vd = containerVehicleData.getData<VehicleData>();

        // 2. Update sensor board data values
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        switch (state){
            case DRIVING:{

                // Update overtaker state info...
                ovt.obstacleDetection(sbd, vd);

                // If overtaker is overriding control values...
                if(ovt.getIsOverriding()) {
                    cout << "Overtaker is OVERRIDING" << endl;
                    vehicleControl = ovt.getOvtControl();
                }
                //... else follow lane-follower instructions...
                else{
                    cout <<"Overtaker NOT overriding" << endl;
                    laneFollowing();
                }

                break;
            }
        }

        // Pack and send control values
        Container control(vehicleControl);
        getConference().send(control);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

/**
 * This method is a data listener that listens to the broadcast made by odsupercomponent for LaneRecommendation
*/
void decisionmaker::DecisionMaker::nextContainer(odcore::data::Container &c) {

    if(c.getDataType() == LaneRecommendation::ID()){
        laneRecommendation = c; //Pointer to which the PacketBroadcaster sends for data.
    }else if(c.getDataType() == SensorBoardData::ID()){
        containerSensorBoardData = c;
    }else if(c.getDataType() == VehicleData::ID()){
        containerVehicleData = c;
    }

}

