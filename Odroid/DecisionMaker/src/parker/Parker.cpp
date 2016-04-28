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

Parker::Parker() :
        parkstate(PHASE0),
        state(FINDOBJECT),
        carPosition(0),
        vc(),
        gapStart(0),
        gapEnd(0),
        isSpot(false),
        isParked(false),
        isAccurate(0),
        controlTemp(),
        objInFront(false),
        turningCounter(0),
        reversing(false),
        lifoQueue(){}

Parker::~Parker(){}

/**
 * Finds a parking spot where the car fits
 */
void Parker::findSpot(SensorBoardData sbd, VehicleData vd, VehicleControl dmVehicleControl) {

    //Check if the car has been turning for a time which means it has been in a curve, then start to find a object again
    if(dmVehicleControl.getSteeringWheelAngle() >  0.5 || dmVehicleControl.getSteeringWheelAngle() < -0.5){
        turningCounter++;
        if(turningCounter == 20) {
            state = FINDOBJECT;
            turningCounter = 0;
        }
    }
    else
        turningCounter = 0;
    //SwitchCase to find a parking spot
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
    //This is a safety check if there is something getting close to the car within the parking and happens
    //when it is going back.
    if(isNotSafe(sbd) && (parkstate == PHASE2 || parkstate == PHASE3 || parkstate == PHASE6) ){
        parkstate = SAFETYSTOP;
    }
    switch(parkstate){
        case PHASE0:{
            //cout << "This is the distance: " << vd.getAbsTraveledPath() << endl;
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
        //Might not be needed for the real car
        case PHASE4:{
            vc = adjustInSpotForward(vd, ADJUST_IN_SPOT_FORWARD);
            break;
        }
        //Might not be needed for the real car
        case PHASE5:{
            vc = adjustInSpotBack(vd, ADJUST_IN_SPOT_BACK);
            break;
        }
        case PHASE6:{
            carPosition = vd.getAbsTraveledPath();
            vc = midOfSpot(sbd);
            break;
        }
        case SAFETYSTOP:{
            vc = goBackToLane(vd);
            break;
        }
    }
    return vc;
}

VehicleControl Parker::midOfSpot(SensorBoardData sbd) {
    if(objInFront) {
        cout << "Get mid of spot" << endl;
        cout << "Infrared: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) << "Ultrasonic: " <<
        sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) << endl;
        inBetweenObjects(sbd);
    }
        // If there isn't any vehicle infront of the car
    else {
        objectBehind(sbd);
    }
    return controlTemp;
}

/*
 * If the car dosn't have an object infront of it it executes this
 */
void Parker::objectBehind(SensorBoardData sbd) {
    if ((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > IRSENSOR_DISTANCE_MIN &&
         sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < IRSENSOR_DISTANCE_MAX)) {
        isAccurate++;
    }
    else
        isAccurate = 0;

    if (isAccurate == FREQUENCY) {
        if ((int) DISTANCE_FROM_BACK_OBJECT * SENSOR_CONVERSION ==
            (int) sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION) {
            cout << "Now parked" << endl;
            controlTemp.setSpeed(0);
            controlTemp.setBrakeLights(true);
            isParked = true;
            reversing = false;
        }
        else if((int) DISTANCE_FROM_BACK_OBJECT * SENSOR_CONVERSION >
                (int)sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION ){
            isAccurate = 0;
            controlTemp.setSpeed(1);
            reversing = false;
        }
        else if((int) DISTANCE_FROM_BACK_OBJECT * SENSOR_CONVERSION <
                (int)sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION) {
            isAccurate = 0;
            controlTemp.setSpeed(-1);
            reversing = true;
        }
    }
}
/*
 * When it is parking in between two object this is executed
 */
void Parker::inBetweenObjects(SensorBoardData sbd) {
    if ((sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) > ULTRASENSOR_DISTANCE_MIN &&
         sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) < ULTRASENSOR_DISTANCE_MAX)
        && (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > IRSENSOR_DISTANCE_MIN &&
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < IRSENSOR_DISTANCE_MAX)) {
        isAccurate++;
    }
    else
        isAccurate = 0;

    if (isAccurate == FREQUENCY) {
        if ((int) sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) * SENSOR_CONVERSION ==
            (int) sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION) {
            cout << "Now parked" << endl;
            controlTemp.setSpeed(0);
            controlTemp.setBrakeLights(true);
            isParked = true;
            reversing = false;
        }
        else if ((int) sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) * SENSOR_CONVERSION >
                 (int) sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION) {
            isAccurate = 0;
            controlTemp.setSpeed(0.3);
            reversing = false;
        }
        else if ((int) sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) * SENSOR_CONVERSION <
                 (int) sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) * SENSOR_CONVERSION) {
            isAccurate = 0;
            controlTemp.setSpeed(-0.3);
            reversing = true;
        }
    }
}

/**
 * Drives the car back for adjusting the hardcoded procedure
 */
VehicleControl Parker::adjustInSpotBack(VehicleData vd, double add) {
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(-0.3);
        controlTemp.setSteeringWheelAngle(-15);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSteeringWheelAngle(0);
        controlTemp.setSpeed(0);
        parkstate = PHASE6;
    }
    return controlTemp;
}

/**
 * Drives the car forward for adjusting the hardcoded procedure
 */
VehicleControl Parker::adjustInSpotForward(VehicleData vd, double add){
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0.3);
        controlTemp.setSteeringWheelAngle(0.5); //30
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
    reversing = true;
    controlTemp.setSteeringWheelAngle(-0.5); // 45
    controlTemp.setBrakeLights(false);
    if(carPosition + add < vd.getAbsTraveledPath()){
        cout << "Backing left finished" << endl;
        controlTemp.setSpeed(0);
        controlTemp.setBrakeLights(true);
        carPosition = carPosition + add;
        parkstate = PHASE6;
    }
    return controlTemp;
}

/**
 * Drives the car straight back into the spot in the hardcoded procedure
 */
VehicleControl Parker::backingStraight(VehicleData vd, double add) {
    reversing = true;
    cout << "Backing straight" << endl;
    if(carPosition + add > vd.getAbsTraveledPath()) {
        controlTemp.setSpeed(-1);
        controlTemp.setBrakeLights(false);
    }
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
    reversing = true;
    if(carPosition + add > vd.getAbsTraveledPath()){
        cout << "Starts back around corner" << endl;
        controlTemp.setSpeed(-1);
        controlTemp.setBrakeLights(false);
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
        controlTemp.setBrakeLights(false);
        controlTemp.setSteeringWheelAngle(0);
    }
    else if(gapEnd + add < vd.getAbsTraveledPath()){
        cout << "Switching to PHASE1" << endl;
        controlTemp.setSpeed(0);
        controlTemp.setBrakeLights(true);
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
    if(vd.getAbsTraveledPath() > gapStart + ENOUGH_SPACE_DISTANCE){
        isAccurate = 0;
        objInFront = true;
        state = ENOUGHSPACE;
        parkstate = PHASE1;
    }

    else {
        //To check if the sensors are in the ranges of where it can find a object
        if ((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT)) > IRSENSOR_DISTANCE_MIN &&
            (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MAX)) {
            isAccurate++;
        }
        else
            isAccurate = 0;
        //To check if the readings are Accurate
        if (isAccurate == FREQUENCY) {
            cout << "*********END OF GAP IS DETECTED****************" << endl;
            gapEnd = vd.getAbsTraveledPath();
            isAccurate = 0;
            objInFront = true;
            state = ENOUGHSPACE;
        }
    }
}
/**
 * Finds the start of the gap
 */
void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    //To check if the sensors are in the ranges of where it can find a gap
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MIN ||
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > IRSENSOR_DISTANCE_MAX){
        isAccurate++;
    }
    else
        isAccurate = 0;
    //To check if the readings are Accurate
    if(isAccurate == FREQUENCY){
        cout << "*********GAP HAS BEEN DETECTED****************" << endl;
        gapStart = vd.getAbsTraveledPath();
        isAccurate = 0;
        state = FINDGAPEND;
    }
}
/**
 * Finds the object
 */
void Parker::findObject(SensorBoardData sbd) {
    //To check if the sensors are in the ranges of where it can find a object
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > IRSENSOR_DISTANCE_MIN &&
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MAX){
        isAccurate++;
    }
    else
        isAccurate = 0;
    //To check if the readings are Accurate and have no noise
    if(isAccurate == FREQUENCY){
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

bool Parker::isReversing(){
    return reversing;
}

/**
 * Method to check if the car is to close to the back object
 */

bool Parker::isNotSafe(SensorBoardData sbd){
    if((sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT) > SENSOR_SAFETY_MIN) &&
       (sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT) < SENSOR_SAFETY_MAX)){
        cout << "ULTRA_SONIC_FRONT_RIGHT" << endl;
        cout << "SENSOR: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK);
        return true;
    }
    if((sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) > SENSOR_SAFETY_MIN) &&
       (sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) < SENSOR_SAFETY_MAX)){
        cout << "ULTRASONIC_FRONT_FORWARD" << endl;
        cout << "SENSOR: " << sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);
        return true;
    }
    if((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > SENSOR_SAFETY_MIN) &&
       (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < SENSOR_SAFETY_MAX)){
        cout << "INFRARED_REAR_BACK" << endl;
        cout << "SENSOR: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK);
        return true;
    }
    if((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > SENSOR_SAFETY_MIN) &&
       (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < SENSOR_SAFETY_MAX)){
        cout << "INFRARED_REAR_RIGHT" << endl;
        cout << "SENSOR: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);
        return true;
    }
    if((sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > SENSOR_SAFETY_MIN) &&
       (sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) < SENSOR_SAFETY_MAX)){
        cout << "INFRARED_FRONT_RIGHT" << endl;
        cout << "SENSOR: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);
        return true;
    }
    return false;
}

/**
 * Method to bring the car back to the lane if the car is to close to the back object
 */
VehicleControl Parker::goBackToLane(VehicleData vd){
    /*if(carPosition + 4 > vd.getAbsTraveledPath()){
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
    }*/
    controlTemp.setSpeed(0);
    controlTemp.setBrakeLights(true);
    return controlTemp;
}