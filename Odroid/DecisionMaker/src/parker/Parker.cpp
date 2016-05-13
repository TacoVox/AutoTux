//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"
#include <cmath>

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
        expectedValue(0),
        controlTemp(),
        objInFront(false),
        turningCounter(0),
        reversing(false),
        outOfSpot(false),
        isInSpot(false){}

Parker::~Parker(){}

/**
 * Finds a parking spot where the car fits
 */
void Parker::findSpot(SensorBoardData sbd, VehicleData vd, VehicleControl dmVehicleControl) {
    //Check if the car has been turning for a time which means it has been in a curve, then start to find a object again
    if(dmVehicleControl.getSteeringWheelAngle() >  0.42 || dmVehicleControl.getSteeringWheelAngle() < -0.42){
        turningCounter++;
        //If it has done that for a second
        if(turningCounter == 30) {
            cout << "There has been a curve" << endl;
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
            findObject(sbd);
            break;
        }
        case FINDGAPSTART: {
            findGapStart(sbd, vd);
            break;
        }
        case FINDGAPEND: {
            findGapEnd(sbd, vd);
            break;
        }
        case CHECKSPACE: {
            enoughSpace();
            break;
        }
    }
}

/**
 * This executes a parking procedure for parking the car
 */
VehicleControl Parker::parallelPark(SensorBoardData sbd, VehicleData vd){
    //This is a safety check if there is something getting close to the car within the parking and happens
    //when it is going back.
    if(!isInSpot && parkstate != PHASE0){
        checkIfInSpot(sbd);
        if(isNotSafe(sbd))
        parkstate = SAFETYSTOP;
    }
    //Switch case for the parking procedure
    switch(parkstate){
        case PHASE0:{
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
            vc = getParallelInSpot(sbd, vd, PARALLEL_IN_SPOT);
            break;
        }
        case PHASE4:{
            carPosition = vd.getAbsTraveledPath();
            vc = midOfSpot(sbd, vd);
            break;
        }
        case SAFETYSTOP:{
            vc = stop();
            break;
        }
    }
    return vc;
}
/**
 * Get the car to a good distance from the objects infront and back of the car
 */
VehicleControl Parker::midOfSpot(SensorBoardData sbd, VehicleData vd) {
    if(objInFront) {
        inBetweenObjects(sbd, vd);
    }
        // If there isn't any vehicle infront of the car
    else {
        objectBehind(sbd, vd);
    }
    return controlTemp;
}

/*
 * If the car dosn't have an object infront of it it executes this
 */
void Parker::objectBehind(SensorBoardData sbd, VehicleData vd) {
    double backSensor = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK);
    //Checks if the car is in a good distance from the object behind
    if (backSensor > SENSOR_DIFFERENCE_NO_FRONT){
        controlTemp.setSpeed(0);
        controlTemp.setBrakeLights(true);
        carPosition = vd.getAbsTraveledPath();
        isParked = true;
        reversing = false;
    }
        //Go more forward until it gets into a good place infront of the object
    else if(DISTANCE_FROM_BACK_OBJECT > backSensor){
        expectedValue = 0;
        controlTemp.setSpeed(1);
        reversing = false;
    }
    else if(DISTANCE_FROM_BACK_OBJECT < backSensor) {
        expectedValue = 0;
        controlTemp.setSpeed(-1);
        reversing = true;
    }
}
/*
 * When it is parking in between two object this is executed
 */
void Parker::inBetweenObjects(SensorBoardData sbd, VehicleData vd) {
    double frontSensor = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);
    double backSensor = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK);
    //If the car is in the accepted range between the object
    if(std::abs(frontSensor - backSensor) < SENSOR_DIFFERENCE_INBETWEEN) {
        cout << "In the middle" << endl;
        controlTemp.setSpeed(0);
        controlTemp.setBrakeLights(true);
        carPosition = vd.getAbsTraveledPath();
        isParked = true;
        reversing = false;

    }
        //Go more forward to get to the accepted place
    else if(frontSensor > backSensor) {
        expectedValue = 0;
        controlTemp.setSpeed(1);
        reversing = false;
    }
        //Go more back to get to the accepted place
    else if(frontSensor < backSensor) {
	   expectedValue = 0;
        controlTemp.setSpeed(-1);
        reversing = true;
    }
}

bool isCloseToBack = false; // Bool for making it go forward of backwards in method getParallelInSpot()
/**
 * Drives the car back into the spot for straighting it up in the hardcoded procedure
 */
VehicleControl Parker::getParallelInSpot(SensorBoardData sbd, VehicleData vd, double add){
        //Do this until it has traveled enough to get in parallel
        if(carPosition + add > vd.getAbsTraveledPath()){
            //If the IR back sensor is bigger than the limit then go back else switch to go forward
            if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > SENSORS_IN_SPOT && !isCloseToBack){
                reversing = true;
                controlTemp.setSteeringWheelAngle(-0.5); // 45
                controlTemp.setBrakeLights(false);
                controlTemp.setSpeed(-1);
            }
            else
                isCloseToBack = true;
            //If the US front sensor is biiger than the limit then go forward else switch to go reversing
            if(sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) > SENSORS_IN_SPOT && isCloseToBack){
                reversing = false;
                controlTemp.setSteeringWheelAngle(0.5); // 45
                controlTemp.setBrakeLights(false);
                controlTemp.setSpeed(1);
            }
            else
                isCloseToBack = false;
        }
            //If the car has traveled enough for being parallel to the road
        else if(carPosition + add < vd.getAbsTraveledPath()){
            controlTemp.setSpeed(0);
            controlTemp.setBrakeLights(true);
            controlTemp.setSteeringWheelAngle(0);
            cout << "Go to get into middle of spot" << endl;
            parkstate = PHASE4;
        }


    return controlTemp;
}
/*
 * Makes the car go straight back (is used in the simulation but not on the real car)
 */
VehicleControl Parker::backingStraight(VehicleData vd, double add) {
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(-1);
        controlTemp.setBrakeLights(false);
        controlTemp.setSteeringWheelAngle(0);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        cout << "Going to getParallel " << endl;
        controlTemp.setSteeringWheelAngle(-0.5);
        carPosition = carPosition + add ;
        parkstate = PHASE3;
    }
    return controlTemp;
}

/**
 * The car backs around the first corner of the object in the parking procedure
 */
VehicleControl Parker::backAroundCorner(VehicleData vd, double add){
    reversing = true;
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(-1);
        controlTemp.setBrakeLights(false);
        controlTemp.setSteeringWheelAngle(0.5); //45
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        cout << "Going to back straight " << endl;
        controlTemp.setSpeed(0);
        controlTemp.setSteeringWheelAngle(0);
        carPosition = carPosition + add ;
        parkstate = PHASE2;
    }
    return controlTemp;
}
/**
 * Adjust the car for the front object before starting the parking procedure
 */
VehicleControl Parker::adjustBeforeParking(VehicleData vd, double add) {
    if(gapEnd + add > vd.getAbsTraveledPath()){
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
 * Looks for the end of the gap
 */
void Parker::findGapEnd(SensorBoardData sbd, VehicleData vd){
    //If the car has traveled enough to be able to park without a object in front
    if(vd.getAbsTraveledPath() > (gapStart + ENOUGH_SPACE_DISTANCE)){
        expectedValue = 0;
        objInFront = false;
        isSpot = true;
        carPosition = vd.getAbsTraveledPath(); //Sets the car position
        parkstate = PHASE1;
        cout << "Found enough space to park" << endl;
    }
    else {
        //To check if the sensors are in the ranges of where it can find a object
        if ((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT)) > IRSENSOR_DISTANCE_MIN &&
            (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MAX)) {
            expectedValue++;
        }
        else
            expectedValue = 0;
        //To check if the readings are Accurate
        if (expectedValue == OCCURRENCE) {
            cout << "*********END OF GAP IS DETECTED****************" << endl;
            gapEnd = vd.getAbsTraveledPath(); // Sets the end of the gap for the calculation of the gap size
            expectedValue = 0;
            objInFront = true;
            state = CHECKSPACE;
        }
    }
}
/**
 * Looks for the start of the gap
 */
void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    //To check if the sensors are in the ranges of where it can find a gap
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MIN ||
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > IRSENSOR_DISTANCE_MAX){
        expectedValue++;
    }
    else
        expectedValue = 0;
    //To check if the readings are Accurate
    if(expectedValue == OCCURRENCE){
        cout << "*********STARTGAP HAS BEEN DETECTED****************" << endl;
        gapStart = vd.getAbsTraveledPath(); //Sets where the start of the gap is for the calculation of the gap size
        expectedValue = 0;
        state = FINDGAPEND;
    }
}
/**
 * Looks for an object
 */
void Parker::findObject(SensorBoardData sbd) {
    //To check if the sensors are in the ranges of where it can find a object
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > IRSENSOR_DISTANCE_MIN &&
            sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < IRSENSOR_DISTANCE_MAX){
        expectedValue++;
    }
    else
        expectedValue = 0;
    //To check if the readings are Accurate and have no noise
    if(expectedValue == OCCURRENCE){
        expectedValue = 0;
        cout << "********OBJECT HAS BEEN FOUND************" << endl;
        state = FINDGAPSTART; //Change state to find the end of the object
    }
}
/**
 * If a spot has been found or not
 */
bool Parker::getFoundSpot() {
    return isSpot;
}
/**
 * If the car is parked or not
 */
bool Parker::getIsParked() {
    return isParked;
}
/**
 * If the car is reversing or not
 */
bool Parker::isReversing(){
    return reversing;
}
/**
 * If the car is back on the lane
 */
bool Parker::isOutOfSpot() {
    return outOfSpot;
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

void Parker::checkIfInSpot(SensorBoardData sbd){
    double us_front_right = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT);
    double ir_front_right = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
    double ir_back_right = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);

    if(sensorClear(us_front_right) && sensorClear(ir_front_right) && sensorClear(ir_back_right)){
        expectedValue++;
    }
    else
        expectedValue = 0;
    if(expectedValue == 3){
        isInSpot = true;
    }
}

bool Parker::sensorClear(double sensor){
    return (sensor > IRSENSOR_DISTANCE_MAX || sensor < IRSENSOR_DISTANCE_MIN);
}

/**
 * Method to bring the car back to the lane after it has parked
 */
bool resume = false;
double tempCarPositino;
VehicleControl Parker::goBackToLane(SensorBoardData sbd, VehicleData vd, double add){
    if(resume){
        if(tempCarPositino + add > vd.getAbsTraveledPath()) {
            controlTemp.setSpeed(1);
            controlTemp.setSteeringWheelAngle(-0.5);
            reversing = false;
        }
        else {
            outOfSpot = true;
            isInSpot = false;
        }

    }
    else if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > IRSENSOR_DISTANCE_MIN){
        controlTemp.setSpeed(-1);
        reversing = true;
        tempCarPositino = vd.getAbsTraveledPath();
    }
    else
        resume = true;
    return controlTemp;
}

VehicleControl Parker::stop() {
    controlTemp.setSpeed(0);
    return controlTemp;
}
