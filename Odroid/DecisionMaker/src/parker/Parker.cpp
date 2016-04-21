//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "parker/Parker.h"

using namespace std;
using namespace parker;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, ENOUGHSPACE};
enum PARKSTATE {PHASE0,PHASE1, PHASE2, PHASE3, PHASE4, PHASE5, SAFETYSTOP};
PARKSTATE parkstate = PHASE0;
STATE state = FINDOBJECT;

double gapStart = 0;
double gapEnd = 0;
bool isSpot = false;
bool isParked = false;
int isAccurate = 0;
VehicleControl controlTemp;

Parker::Parker() :
    carPosition(0), vc(){}

Parker::~Parker(){}

/**
 * Finds a parking spot where the car fits
 */
void Parker::findSpot(SensorBoardData sbd, VehicleData vd) {
    switch (state) {
        case FINDOBJECT: {
            isSpot = false;
            cout << "FIND OBJECT" << endl;
            findObject(sbd);
            break;
        }
        case FINDGAPSTART: {
            cout << "FIND START OF GAP" << endl;
            findGapStart(sbd, vd);
            break;
        }
        case FINDGAPEND: {
            cout << "FIND END OF GAP" << endl;
            findGapEnd(sbd, vd);
            break;
        }
        case ENOUGHSPACE: {
            cout << "ENOUGH SPACE" << endl;
            enoughSpace();
            break;
        }
    }
}

/**
 * This executes a hardcoded procedure for parking the car
 */
VehicleControl Parker::parallelPark(SensorBoardData sbd, VehicleData vd){
    if(isNotSafe((sbd))){
        parkstate = SAFETYSTOP;
    }
    switch(parkstate){
        case PHASE0:{
            cout << "This is the distance: " << vd.getAbsTraveledPath() << endl;
            vc = adjustBeforeParking(vd, ADJUST_BEFORE_PARKING);
            break;
        }
        case PHASE1:{
            vc = backAroundCorner(vd, BACK_AROUND_CORNER);
            break;
        }
        case PHASE2:{
            vc = backingStraight(vd, BACKING_STRAIGHT);
            break;
        }

        case PHASE3:{
            vc = backingLeft(vd, BACKING_LEFT);
            break;
        }

        case PHASE4:{
            vc = adjustInSpotForward(vd, ADJUST_IN_SPOT_FORWARD);
            break;
        }
        /*
        case PHASE5:{
            vc = adjustInSpotBack(vd, ADJUST_IN_SPOT_BACK);
            break;
        }
        case SAFETYSTOP:{
            vc = goBackToLane(vd);
            break;
        }*/
    }
    return vc;
}

/**
 * Drives the car back for adjusting the hardcoded procedure
 */
VehicleControl Parker::adjustInSpotBack(VehicleData vd, double add) {
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(-1);
        controlTemp.setSteeringWheelAngle(-15);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        isParked = true;
    }
    return controlTemp;
}

/**
 * Drives the car forward for adjusting the hardcoded procedure
 */
VehicleControl Parker::adjustInSpotForward(VehicleData vd, double add){
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(1);
        controlTemp.setSteeringWheelAngle(0); //30
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        controlTemp.setSteeringWheelAngle(0); //30
        carPosition = carPosition + add;
        parkstate = PHASE5;
    }
    return controlTemp;
}

/**
 * Drives the car back into the spot for straighting it up in the hardcoded procedure
 */
VehicleControl Parker::backingLeft(VehicleData vd, double add){
    controlTemp.setSteeringWheelAngle(-0.5); // 45
    if(carPosition + add < vd.getAbsTraveledPath()){
        cout << "Backing left finished" << endl;
        controlTemp.setSpeed(0);
        carPosition = carPosition + add;
        parkstate = PHASE4;
    }
    return controlTemp;
}

/**
 * Drives the car straight back into the spot in the hardcoded procedure
 */
VehicleControl Parker::backingStraight(VehicleData vd, double add) {
    if(carPosition + add > vd.getAbsTraveledPath())
        controlTemp.setSpeed(-1);
    else {
        parkstate = PHASE3;
        carPosition = carPosition + add;
    }
    return controlTemp;
}

/**
 * The car backs around the first corner of the object in the hardcoded procedure
 */
VehicleControl Parker::backAroundCorner(VehicleData vd, double add){
    if(carPosition + add > vd.getAbsTraveledPath()){
        cout << "Starts back around corner" << endl;
        controlTemp.setSpeed(-1);
        controlTemp.setSteeringWheelAngle(0.5); //45
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        controlTemp.setSteeringWheelAngle(0);
        carPosition = carPosition + add ;
        parkstate = PHASE2;
    }
    return controlTemp;
}
/**
 * Adjust the car for the front object before starting the hardcoded procedure
 */
VehicleControl Parker::adjustBeforeParking(VehicleData vd, double add) {
    if(gapEnd + add > vd.getAbsTraveledPath()){
        cout << "Inside adjustbeforeparking " << endl;
        controlTemp.setSpeed(1);
        controlTemp.setSteeringWheelAngle(0);
    }
    else if(gapEnd + add < vd.getAbsTraveledPath()){
        cout << "Switching to PHASE1" << endl;
        controlTemp.setSpeed(0);
        parkstate = PHASE1;
        carPosition = vd.getAbsTraveledPath();
    }
    return controlTemp;
}

/**
 * Checks if the gap is big enough to park in
 */
void Parker::enoughSpace(){
    cout << "ENDGAP: " << gapEnd << " Startgap: " << gapStart << " Space site: " << gapEnd - gapStart << endl;
    if((gapEnd - gapStart) > SPOT_SIZE){
        cout << "******************ENOUGHSPACE TO PARK***************" << endl;
        isSpot = true;
        parkstate = PHASE0;
    }
    else
        state = FINDOBJECT;
}

/**
 * Finds end of the gap
 */
void Parker::findGapEnd(SensorBoardData sbd, VehicleData vd){
    if((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT)) > 0 &&
            (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 20)) {
        gapEnd = vd.getAbsTraveledPath();
        isAccurate++;
    }
    else
        isAccurate = 0;
    //To check if the readings are Accurate and have no noise
    if(isAccurate == 5){
        cout << "*********END OF GAP IS DETECTED****************" << endl;
        isAccurate = 0;
        state = ENOUGHSPACE;
    }
}
/**
 * Finds the start of the gap
 */
void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 0 ||
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 20){
        gapStart = vd.getAbsTraveledPath();
        isAccurate++;
    }
    else
        isAccurate = 0;
    //To check if the readings are Accurate and have no noise
    if(isAccurate == 5){
        cout << "*********GAP HAS BEEN DETECTED****************" << endl;
        isAccurate = 0;
        state = FINDGAPEND;
    }
}
/**
 * Finds the object
 */
void Parker::findObject(SensorBoardData sbd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0 &&
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 20){
        isAccurate++;
    }
    else
        isAccurate = 0;
    //To check if the readings are Accurate and have no noise
    if(isAccurate == 5){
        isAccurate = 0;
        cout << "********OBJECT HAS BEEN FOUND************" << endl;
        state = FINDGAPSTART;
    }
}
/**
 * If a spot has been found or not
 */
bool Parker::getFoundSpot(){
    return isSpot;
}
/**
 * Returns if the car has finished parking
 */
bool Parker::getIsParked() {
    return isParked;
}

/**
 * Method to check if the car is to close to the back object
 */
bool Parker::isNotSafe(SensorBoardData sbd){
    return((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > 0.5) &&
           (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < 0.8));
}
/**
 * Method to bring the car back to the lane if the car is to close to the back object
 */
VehicleControl Parker::goBackToLane(VehicleData vd){
    if(carPosition + 4 > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0.6);
        controlTemp.setSteeringWheelAngle(-25);
    }
    else if(carPosition + 7 > vd.getAbsTraveledPath()){
        controlTemp.setSteeringWheelAngle(30);
    }
    else {
        state = FINDOBJECT;
        parkstate = PHASE0;
        isSpot = false;
    }
    return controlTemp;
}