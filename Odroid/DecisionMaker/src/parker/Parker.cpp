//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

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
    foundSpot = false;
}
void Parker::tearDown(){
    cout << "This is when Parker stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parker::body() {

    gapStart = 0;
    gapEnd = 0;


    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        absTraveled = 0;
        parkPosition = 0;
        int max = 0;
        state = FINDOBJECT;
        //If the decisionmaker is in PARKING state
        while(*parking){
            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData>();

            // 2. Get most recent sensor board data describing virtual sensor data:
            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData>();

            absTraveled = vd.getAbsTraveledPath();

            if(isSafe(sbd)){
                cout << "SafetyStop Has been activated" << endl;
                state = SAFETYSTOP;
            }

            switch (state){
                case FINDOBJECT:{
                    findObject(sbd);
                    break;
                }
                case FINDGAPSTART:{
                    findGapStart(sbd, vd);
                    break;
                }
                case FINDGAPEND:{
                    findGapEnd(sbd, vd);
                    break;
                }
                case ENOUGHSPACE:{
                    enoughSpace();
                    break;
                }
                case ADJUST:{
                    adjustCar(absTraveled);
                    break;
                }
                case PARK:{
                    parallelPark();
                    break;
                }
                case SAFETYSTOP:{
                    goBackToLane();
                    break;
                }
            }
            *parkingControler = vc; //Sends the controldata to the DecisionMaker
        }
    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
/**
 * Switch for the different phases that the car will do for the parking
 */
void Parker::parallelPark(){
    switch(parkstate){
        case PHASE1:{
            backAroundObj();
            break;
        }
        case PHASE2:{
            backingStraight();
            break;
        }
        case PHASE3:{
            backingLeft();
            break;
        }
        case PHASE4:{
            adjustInSpotForward();
            break;
        }
        case PHASE5:{
            adjustInSpotBack();
            break;
        }

    }
}

/**
 * Returns true or false if the back of the car gets to close to the object behind
 */
bool Parker::isSafe(SensorBoardData sbd){
    return((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > 0.5) &&
            (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < 1));
}
/**
 * Method to go back to the lane if there is something wrong with the parking
 */
void Parker::goBackToLane(){
    if(parkPosition + 4 > absTraveled){
        vc.setSpeed(0.6);
        vc.setSteeringWheelAngle(-25);
    }
    else if(parkPosition + 7 > absTraveled){
        vc.setSteeringWheelAngle(30);
    }
    else {
        state = FINDOBJECT;
        foundSpot = false;
    }
}
/**
 * Adjust the car to be in a good place inside the spot
 */
void Parker::adjustInSpotBack() {
    if(parkPosition + 0.5 > absTraveled){
        vc.setSpeed(-0.3);
        vc.setSteeringWheelAngle(-15);
    }
    else if(parkPosition + 0.5 < absTraveled){
        vc.setSpeed(0);
    }
}
/**
 * Adjust to be straighter Forward
 */
void Parker::adjustInSpotForward(){
    if(parkPosition + 1.5 > absTraveled){
        vc.setSpeed(0.3);
        vc.setSteeringWheelAngle(30);
    }
    else if(parkPosition + 1.5 < absTraveled){
        vc.setSpeed(0);
        vc.setSteeringWheelAngle(30);
        parkPosition += 1.5;
        parkstate = PHASE5;
    }
}
/**
 * Placing the car inside the spot
 */
void Parker::backingLeft(){
    vc.setSteeringWheelAngle(-45);
    if(parkPosition + 2.5 < absTraveled){
        vc.setSpeed(0);
        parkPosition += 2.5;
        parkstate = PHASE4;
    }
}
/**
 * Goes back straight
 */
void Parker::backingStraight() {
    if(parkPosition + 2 > absTraveled)
        vc.setSpeed(-0.5);
    else {
        parkstate = PHASE3;
        parkPosition += 2;
    }
}
/**
 * Backs around the bottom left corner of the object
 */
void Parker::backAroundObj(){
    if(parkPosition + 5 > absTraveled){
        vc.setSpeed(-0.6);
        vc.setSteeringWheelAngle(45);
    }
    else if(parkPosition + 5 < absTraveled){
        vc.setSpeed(0);
        vc.setSteeringWheelAngle(0);
        parkPosition += 5;
        parkstate = PHASE2;
    }
}
/**
 * Adjusts the car before the parking begin
 */
void Parker::adjustCar(double absTraveled) {
    cout << "Enters ADJUST" << endl;
    if(gapEnd + 2 > absTraveled){
        vc.setSpeed(.6);
        vc.setSteeringWheelAngle(0);
    }
    else if(gapEnd + 2 < absTraveled){
        vc.setSpeed(0);
        state = PARK;
        parkstate = PHASE1;
        parkPosition = absTraveled;
    }
}
/**
 * Calculates if the Space is enough to park in
 */
void Parker::enoughSpace(){
    if((gapEnd - gapStart) > 6){
        state = ADJUST;
        foundSpot = true;
    }
    else
        state = FINDOBJECT;
}
/**
 * Finds the end of the gap when noticeing another object
 */
void Parker::findGapEnd(SensorBoardData sbd, VehicleData vd){
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0) {
        cout << "Found gap END method---------------------------" << endl;
        gapEnd = vd.getAbsTraveledPath();
        state = ENOUGHSPACE;
    }
}
/*
 * Finds the first gap from the detected object and changes state
 */
void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 0){
        gapStart = vd.getAbsTraveledPath();
        state = FINDGAPEND;
        cout << "Found gap START method---------------------------" << endl;
    }
}
/**
 * This method detects if there is an object and change state
 */

void Parker::findObject(SensorBoardData sbd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0){
        cout << "object found method---------------------------" << endl;
        state = FINDGAPSTART;
    }
}

/**
 * This method sets the bool parking passed from DecisionMaker
 */
void Parker::setParking(std::shared_ptr<bool> parking){
    this->parking = parking;
}
/**
 * This sets the pointer to which is the VehicleControl
 */
void Parker::setParkingControler(std::shared_ptr<Container> parkingControler){
    this->parkingControler = parkingControler;
}
/**
 * This returns a bool if there has been a parking spot found
 */
bool Parker::getFoundSpot(){
    return foundSpot;
}
