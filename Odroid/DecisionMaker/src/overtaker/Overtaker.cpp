//
// Created by marco on 2016-04-07.
//
#include <iostream>
#include "overtaker/Overtaker.h"

using namespace odcore::data;                 // Allows 'Container'

using namespace automotive::miniature;          // Sensor Board Data
using namespace overtaker;

/* @doc Constructor.
 * */
Overtaker::Overtaker():
    isOverridingControls(false), traveledPath(0), state(FREE_LANE){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker sub-state
 * */
void Overtaker::obstacleDetection(automotive::miniature::SensorBoardData sensorData, automotive::VehicleData vehicleData) {

    switch(state){
        case FREE_LANE:{
            cout << "Overtaker: run FREE-LANE obstacle detection" << endl;
            if(isObstacleOnLane(sensorData, 15.0)){
                cout << "OBSTACLE DETECTED" << endl;
                state = APPROACHING;
            }
            break;
        }
        case APPROACHING:{
            cout << "Overtaker: run APPROACHING obstacle detection" << endl;

            if(isObstacleOnLane(sensorData, 7.0)){
                cout << "SWITCHING TO LEFT LANE" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                isOverridingControls = true;
                state = LEFT_SWITCH;
            }
            break;
        }
        case LEFT_SWITCH:{
            cout << "Overtaker: run LEFT-SWITCH obstacle detection and car control" << endl;
            switchToLeftLane(vehicleData, traveledPath, 3.4);
            break;
        }
        case PARALLEL:{
            cout << "Overtaker: run PARALLEL obstacle detection" << endl;
            if(isRightLaneClear(sensorData)){
                traveledPath = vehicleData.getAbsTraveledPath();
                isOverridingControls = true;
                state = RIGHT_SWITCH;
            }
            break;
        }
        case RIGHT_SWITCH:{
            cout << "Overtaker: run RIGHT-SWITCH obstacle detection" << endl;
            switchToRightLane(vehicleData, traveledPath, 3.4);
            break;
        }
    }

}

/* @doc Getter for boolean value 'isOverridingControls'
 * */
bool Overtaker::getIsOverriding(){
    return isOverridingControls;
}

VehicleControl Overtaker::getOvtControl() {
    return ovtControl;
}

bool Overtaker::isObstacleOnLane(SensorBoardData sbd, const double range){

    double frontUsSensor = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);
    cout << "US Sensor: " << frontUsSensor << endl;

    if(frontUsSensor < range && frontUsSensor > 0){
        return true;
    }

    return false;
}

/* @doc Moves the car towards left lane for a distance
 *      of 'maxTrv' starting from 'trvStart'.
 * */
void Overtaker::switchToLeftLane(automotive::VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = PARALLEL;
        return;
    }

    // ... else keep steering left
    ovtControl.setSteeringWheelAngle(-30);
    ovtControl.setSpeed(2);
}

/* @doc Moves the car towards right lane for a distance
 *      of 'maxTrv' starting from 'trvStart'.
 * */
void Overtaker::switchToRightLane(automotive::VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = FREE_LANE;
        return;
    }

    // ... else keep steering left
    ovtControl.setSteeringWheelAngle(30);
    ovtControl.setSpeed(2);

}

/* @doc Returns TRUE if the right lane is free
 *      and car can return to right lane tp complete
 *      overtaking maneuver
 * */
bool Overtaker::isRightLaneClear(SensorBoardData sensorData){

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, 8.0);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, 7.0);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, 7.0);

    if(!us_fr && !ir_fr && !ir_rr){
        return true;
    }

    return false;
}

/* @doc Returns true if 'sensorNumber' detects an obstacle within range.
 * */
bool Overtaker::isObstacleDetected(automotive::miniature::SensorBoardData sensorData, const double sensorNumber, const double range) {

    double sensorValue = sensorData.getValueForKey_MapOfDistances(sensorNumber);

    if(sensorValue >= 0 && sensorValue < range){
        return true;
    }

    return false;

}