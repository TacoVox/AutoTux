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

// Shared pointer to Container
VehicleControl vehicleControl;

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker"),
        ovt(), parker(), containerVehicleData(), containerSensorBoardData(), laneRecommendation(), stopCounter(0){
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

    if(stopCounter > 0) {

        if(stopCounter == 60) {
            cout << "WAKING UP" << endl;
            vehicleControl.setSpeed(2);
            stopCounter = 0;
        }

        else {
            cout << "SLEEPING..." << endl;
            stopCounter++;
        }
    }

    else if(getDistanceToLine() == -1){
        vehicleControl.setSpeed(2);
    }

    else if(getDistanceToLine() < 50) {
        vehicleControl.setSpeed(0);
        stopCounter = 1;
    }

    else if(getDistanceToLine() < 150) {
        vehicleControl.setSpeed(1);
    }

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
    vehicleControl.setSpeed(2.0);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        // 1. Update sensor board data values
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        // 2. Update vehicle data values
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        laneRecommendation = getKeyValueDataStore().get(autotux::LaneRecommendation::ID());

        //cout << "SensorValues: " <<  sbd.getValueForKey_MapOfDistances(2) << endl;
        //cout << "Distance: " << vd.getAbsTraveledPath() << endl;

        switch (state){
            case DRIVING:{
                ovt.obstacleDetection(sbd, vd, vehicleControl);

                    // If overtaker is overriding control values...
                if(ovt.getIsOverriding()) {
                    //cout << "DM: OVERTAKER is OVERRIDING" << endl;
                    vehicleControl = ovt.getOvtControl();
                }
                    //... else follow lane-follower instructions...
                else{
                    //cout <<"DM: LANE FOLLOWER Instructions" << endl;
                    laneFollowing();
                }

                break;
            }
            case PARKING:{

                if(parker.getFoundSpot()){
                    if(!parker.getIsParked()) {
                        vehicleControl = parker.parallelPark(sbd, vd);
                    }
                    else {
                        cout << "NOW PARKED!!!" << endl;
                        break;
                    }
                }
                else{
                    //parker.findSpot(sbd, vd);
                    vehicleControl.setSpeed(2);
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
/*
void decisionmaker::DecisionMaker::nextContainer(odcore::data::Container &c) {

    if(c.getDataType() == LaneRecommendation::ID()){
        laneRecommendation = c; //Pointer to which the PacketBroadcaster sends for data.
    }else if(c.getDataType() == SensorBoardData::ID()){
        containerSensorBoardData = c;
    }else if(c.getDataType() == VehicleData::ID()){
        containerVehicleData = c;
    }

}

*/