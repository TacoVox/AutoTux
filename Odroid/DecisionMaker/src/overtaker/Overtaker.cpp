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
    enterSwitchAngle(0), leftLane(false){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker sub-state
 * */
void Overtaker::obstacleDetection(SensorBoardData sensorData, VehicleData vehicleData, VehicleControl dmControl) {

    switch(state){

        case FREE_LANE:{
            cout << "Overtaker: FREE-LANE" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;

            // If close to obstacle switch lane to overtake...
            if(isObstacleOnLane(sensorData, OVT_TRIGGER)){
                cout << "SWITCHING TO LEFT LANE" << endl;

                enterSwitchAngle = dmControl.getSteeringWheelAngle();
                cout << "**** angle : " << enterSwitchAngle << endl;

                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setSpeed(1);
                isOverridingControls = true;
                leftLane = true;

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
            switchToLeftLane(vehicleData, traveledPath, LEFT_SWITCH_DIST);
            break;
        }
        case LEFT_SWITCH_LT:{
            cout << "Overtaker: LEFT-SWITCH on LEFT CURVE" << endl;
            switchToLeftLane(vehicleData, traveledPath, LEFT_SWITCH_LT_DIST);
            break;
        }
        case LEFT_SWITCH_RT:{
            cout << "Overtaker: LEFT-SWITCH on RIGHT CURVE" << endl;
            switchToLeftLane(vehicleData, traveledPath, LEFT_SWITCH_RT_DIST);
            break;
        }
        case ADJUST_LEFT_SWICH:{
            cout << "Overtaker: ADJUST LEFT SWITCH" << endl;
            adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_DIST);
            break;
        }
        case ADJUST_LEFT_SWITCH_LT:{
            cout << "Overtaker: ADJUST LEFT SWITCH on Left Curve" << endl;
            adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_LT_DIST);
            break;
        }
        case ADJUST_LEFT_SWITCH_RT:{
            cout << "Overtaker: ADJUST LEFT SWITCH on Right Curve" << endl;
            adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_RT_DIST);
            break;
        }
        case LEFT_LANE:{
            cout << "Overtaker: LEFT LANE" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            if(isParallelToObstacle(sensorData)){
                state = PARALLEL;
            }

            break;
        }
        case PARALLEL:{
            cout << "Overtaker: PARALLEL " << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;
            if(isRightLaneClear(sensorData)){
                cout << "SWITCHING TO RIGHT LANE" << endl;
                ovtControl.setSpeed(1);
                // ***enter-switch-angle
                enterSwitchAngle = dmControl.getSteeringWheelAngle();

                cout << "**** angle : " << enterSwitchAngle << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                isOverridingControls = true;
                leftLane = false;

                // *** Differentiate right-switches
                if(enterSwitchAngle < -0.1){
                    state = RIGHT_SWITCH_LT;
                }else if(enterSwitchAngle > 0.1){
                    state = RIGHT_SWITCH_RT;
                }else{
                    state = RIGHT_SWITCH;
                }

            }
            break;
        }
        case RIGHT_SWITCH:{
            cout << "Overtaker: RIGHT-SWITCH" << endl;
            //cout << "ANGLE : " << dmControl.getSteeringWheelAngle() << endl;

            switchToRightLane(vehicleData, traveledPath, RIGHT_SWITCH_DIST);
            break;
        }
        case RIGHT_SWITCH_LT:{
            cout << "Overtaker: RIGHT-SWITCH on Left Turn" << endl;

            switchToRightLane(vehicleData, traveledPath, RIGHT_SWITCH_LT_DIST);
            break;
        }
        case RIGHT_SWITCH_RT:{
            cout << "Overtaker: RIGHT-SWITCH on Right turn" << endl;

            switchToRightLane(vehicleData, traveledPath, RIGHT_SWITCH_RT_DIST);
            break;
        }
        case ADJUST_RIGHT_SWITCH:{
            cout << "Overtaker: ADJUST RIGHT-SWITCH" << endl;
            adjustRightSwitch(vehicleData, traveledPath, ADJUST_R_S_DIST);
            break;
        }
        case ADJUST_RIGHT_SWITCH_LT:{
            cout << "Overtaker: ADJUST RIGHT-SWITCH on Left Curve" << endl;
            adjustRightSwitch(vehicleData, traveledPath, ADJUST_R_S_LT_DIST);
            break;
        }
        case ADJUST_RIGHT_SWITCH_RT:{
            cout << "Overtaker: ADJUST RIGHT-SWITCH on Right Curve" << endl;
            adjustRightSwitch(vehicleData, traveledPath, ADJUST_R_S_RT_DIST);
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
void Overtaker::switchToLeftLane(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){

        ovtControl.setSpeed(1.0);
        traveledPath = vehicleData.getAbsTraveledPath();

        if(state == LEFT_SWITCH_RT){
            state = ADJUST_LEFT_SWITCH_RT;
        }else if(state == LEFT_SWITCH_LT){
            state = ADJUST_LEFT_SWITCH_LT;
        }else{
            state = ADJUST_LEFT_SWICH;
        }

        return;
    }

    // ... else keep steering left
    if(state == LEFT_SWITCH_RT){
        ovtControl.setSteeringWheelAngle(-0.45);
    }else{
        ovtControl.setSteeringWheelAngle(-0.5235);
    }


}

void Overtaker::adjustLeftSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // To-Do...
    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = LEFT_LANE;
        return;
    }

    //... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5);
}

bool Overtaker::isParallelToObstacle(SensorBoardData sensorData) {

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, US_SENSOR_RANGE);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, IR_SENSOR_RANGE);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, IR_SENSOR_RANGE);

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
void Overtaker::switchToRightLane(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled :" << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){

        ovtControl.setSpeed(1.0);
        traveledPath = vehicleData.getAbsTraveledPath();

        if(state == RIGHT_SWITCH_LT){
            state = ADJUST_RIGHT_SWITCH_LT;
        }else if(state == RIGHT_SWITCH_RT){
            state = ADJUST_RIGHT_SWITCH_RT;
        }else{
            state = ADJUST_RIGHT_SWITCH;
        }

        return;
    }

    // ... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5235);
}

void Overtaker::adjustRightSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        isOverridingControls = false;
        state = FREE_LANE;
        return;
    }

    //... else keep steering left
    ovtControl.setSteeringWheelAngle(-0.5);
}

/* @doc Returns TRUE if the right lane is free
 *      and car can return to right lane tp complete
 *      overtaking maneuver
 * */
bool Overtaker::isRightLaneClear(SensorBoardData sensorData){

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, US_SENSOR_RANGE);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, IR_SENSOR_RANGE);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, IR_SENSOR_RANGE);

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

bool Overtaker::isLeftLane() {
    return leftLane;
}
