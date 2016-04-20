//
// Created by marco on 2016-04-07.
//
#include <iostream>
#include "overtaker/Overtaker.h"

using namespace odcore::data;                   // Allows 'Container'

using namespace automotive;                     // Allows 'Vehicle Control' and 'Vehicle Data'
using namespace automotive::miniature;          // Sensor 'Board Data'
using namespace overtaker;

/* @doc Constructor.
 * */
Overtaker::Overtaker():
    isOverridingControls(false), traveledPath(0), state(FREE_LANE),
    enterSwitchAngle(0){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker sub-state
 * */
void Overtaker::obstacleDetection(SensorBoardData sensorData, VehicleData vehicleData, VehicleControl dmControl) {

    switch(state){
        case FREE_LANE:{
            //cout << "Overtaker: FREE-LANE" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;

            if(isObstacleOnLane(sensorData, 15.0)){
                cout << "OBSTACLE DETECTED" << endl;
                state = APPROACHING;
            }
            break;
        }
        case APPROACHING:{
            //cout << "Overtaker: APPROACHING" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;

            if(isObstacleOnLane(sensorData, 8.0)){
                cout << "SWITCHING TO LEFT LANE" << endl;

                enterSwitchAngle = dmControl.getSteeringWheelAngle();
                cout << "**** angle : " << dmControl.getSteeringWheelAngle() << endl;

                traveledPath = vehicleData.getAbsTraveledPath();
                isOverridingControls = true;

                // If we are on a left curve...
                if(enterSwitchAngle < -0.1){
                    state = LEFT_SWITCH_LT;
                }
                // ... else if we are on a right curve...
                else if(enterSwitchAngle > 0.1){
                    state = LEFT_SWITCH_RT;
                }
                // ... else we are on a straight lane
                else{
                    state = LEFT_SWITCH;
                }
            }
            break;
        }
        case LEFT_SWITCH:{
            cout << "Overtaker: LEFT-SWITCH" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            switchToLeftLane(vehicleData, dmControl, traveledPath, 2.6);
            break;
        }
        case LEFT_SWITCH_LT:{
            cout << "Overtaker: LEFT-SWITCH on LEFT CURVE" << endl;
            switchToLeftLane(vehicleData, dmControl, traveledPath, 3.6);
            break;
        }
        case LEFT_SWITCH_RT:{
            cout << "Overtaker: LEFT-SWITCH on RIGHT CURVE" << endl;
            switchToLeftLane(vehicleData, dmControl, traveledPath, 3.2);
            break;
        }
        case LEFT_LANE:{
            //cout << "Overtaker: LEFT LANE" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            if(isParallelToObstacle(sensorData)){
                state = PARALLEL;
            }

            break;
        }
        case PARALLEL:{
            //cout << "Overtaker: PARALLEL " << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            if(isRightLaneClear(sensorData)){
                cout << "SWITCHING TO RIGHT LANE" << endl;
                cout << "**** angle : " << dmControl.getSteeringWheelAngle() << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                isOverridingControls = true;

                state = RIGHT_SWITCH;
            }
            break;
        }
        case RIGHT_SWITCH:{
            cout << "Overtaker: RIGHT-SWITCH" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            switchToRightLane(vehicleData, dmControl, traveledPath, 2.6);
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
void Overtaker::switchToLeftLane(VehicleData vehicleData, VehicleControl currControl, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = LEFT_LANE;
        return;
    }

    // ... else keep steering left
    if(state == LEFT_SWITCH_RT){
        ovtControl.setSteeringWheelAngle(-enterSwitchAngle);
    }else {
        ovtControl.setSteeringWheelAngle(-0.5);
    }

    ovtControl.setSpeed(2);


}

bool Overtaker::isParallelToObstacle(SensorBoardData sensorData) {

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, 8.0);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, 7.0);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, 7.0);

    // If US_front-right detects a gap while right-side infrared detect the obstacle...
    if(!us_fr && ir_fr && ir_rr){
        cout << "CAR is parallel to obstacle" << endl;
        return true;
    }

    return false;
}

/* @doc Moves the car towards right lane for a distance
 *      of 'maxTrv' starting from 'trvStart'.
 * */
void Overtaker::switchToRightLane(VehicleData vehicleData, VehicleControl currControl, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = FREE_LANE;
        return;
    }

    // ... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5235);
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
bool Overtaker::isObstacleDetected(SensorBoardData sensorData, const double sensorNumber, const double range) {

    double sensorValue = sensorData.getValueForKey_MapOfDistances(sensorNumber);

    if(sensorValue >= 0 && sensorValue < range){
        return true;
    }

    return false;

}