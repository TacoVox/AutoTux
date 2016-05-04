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
    ovtControl(), isOverridingControls(false), traveledPath(0), state(FREE_LANE),
    leftLane(false), consecReadings(0), min_us_fr(OVT_TRIGGER){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker sub-state
 * */
void Overtaker::obstacleDetection(SensorBoardData sensorData, VehicleData vehicleData, VehicleControl dmControl) {

    switch(state){

        case FREE_LANE:{

            // If close to obstacle switch lane to overtake...
            if(isObstacleOnLane(sensorData, OVT_TRIGGER)){

                cout << "SWITCHING TO LEFT-SWITCH" << endl;

                traveledPath = vehicleData.getAbsTraveledPath();
                min_us_fr = OVT_TRIGGER;
                ovtControl.setSpeed(1);
                isOverridingControls = true;
                ovtControl.setFlashingLightsLeft(true);
                state = LEFT_SWITCH;
            }
            break;
        }
        case LEFT_SWITCH:{

            if(isOnLeftLane(sensorData, vehicleData, traveledPath, 0.0)){
                cout << "SWITCHING TO ADJUST LEFT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                state = ADJUST_LEFT_SWITCH;
            }
            break;
        }
        case ADJUST_LEFT_SWITCH:{

            if(adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_DIST)){
                isOverridingControls = false;
                state = TMP_SWITCH;
                leftLane = true;
            }
            break;
        }
        case TMP_SWITCH:{

            if(isRightLaneClear(sensorData)){

                cout << "SWITCHING TO RIGHT LANE" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setSpeed(1);
                ovtControl.setFlashingLightsRight(true);
                isOverridingControls = true;
                state = RIGHT_SWITCH;

                }
            break;
        }

        case LEFT_LANE:{

            if(isParallelToObstacle(sensorData)){
                consecReadings++;

                if(consecReadings >= NUM_OF_READINGS){
                    cout << "SWITCHING TO PARALLEL" << endl;
                    state = PARALLEL;
                    consecReadings = 0;
                }
            }

            break;
        }
        case PARALLEL:{

            if(isRightLaneClear(sensorData)){

                cout << "SWITCHING TO RIGHT LANE" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setSpeed(1);
                ovtControl.setFlashingLightsRight(true);
                isOverridingControls = true;
                state = RIGHT_SWITCH;

                }
            break;
        }
        case RIGHT_SWITCH:{

            if(switchToRightLane(vehicleData, traveledPath, RIGHT_SWITCH_DIST)){
                cout << "SWITCHING TO ADJUST RIGHT SWITCH" << endl;
                ovtControl.setSpeed(1.0);
                ovtControl.setFlashingLightsRight(false);
                traveledPath = vehicleData.getAbsTraveledPath();
                state = ADJUST_RIGHT_SWITCH;
            }
            break;
        }
        case ADJUST_RIGHT_SWITCH:{

            if(adjustRightSwitch(vehicleData, traveledPath, ADJUST_R_S_DIST)){
                cout << "SWITCHING TO FREE LANE" << endl;
                isOverridingControls = false;
                state = FREE_LANE;
                leftLane = false;
            }
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

    if(frontUsSensor < range && frontUsSensor > 0){
	cout << "*****FRONT US sensor: " << frontUsSensor << endl;

        consecReadings++;

        if(consecReadings >= NUM_OF_READINGS){

            consecReadings = 0;
            return true;
        }
    }
    return false;
}

/* @doc Returns true if FRONT_RIGHT_US sensor distance signals we passed a corner
 *      obstacle.
 * */
bool Overtaker::isOnLeftLane(SensorBoardData sbd, VehicleData vehicleData, const double trvStart, const double threshold){

    double frontRightUsSensor = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_RIGHT);
    cout << "*** VAL : " << frontRightUsSensor << " - Min : " << min_us_fr << endl;

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;
    cout << "traveled : " << traveled << endl;

    // Check if obstacle rear-left corner was passed by the car...
    if(frontRightUsSensor < min_us_fr && frontRightUsSensor > 0){
        min_us_fr = frontRightUsSensor;
    }else if(frontRightUsSensor > 0){
        consecReadings++;
        if(consecReadings > NUM_OF_READINGS && traveled > threshold){
            consecReadings = 0;
            return true;
        }
    }

    // ... else keep steering left
    ovtControl.setSteeringWheelAngle(-0.5235);

    return false;
}

/* @doc ...
 * */
bool Overtaker::adjustLeftSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // To-Do...
    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
 	    //traveledPath = vehicleData.getAbsTraveledPath();
        return true;
    }

    //... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5);

    return false;
}

/* @doc...
 * */
bool Overtaker::isParallelToObstacle(SensorBoardData sensorData) {

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, US_FRONT_RIGHT_RANGE);
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
bool Overtaker::switchToRightLane(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled :" << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        return true;
    }

    // ... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5235);
    return false;
}

bool Overtaker::adjustRightSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    cout << "traveled " << traveled << endl;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        return true;
    }

    //... else keep steering left
    ovtControl.setSteeringWheelAngle(-0.5);
    return false;
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

    //cout << "IS RIGHT LANE CLEAR: US_FR : " << us_fr << " - IR_FR : " << ir_fr << " - IR_RR : " << ir_rr << endl;

    if(!us_fr && !ir_fr && !ir_rr){
        consecReadings++;
        cout << "CONSEC : " << consecReadings << endl;

        if(consecReadings >= NUM_OF_READINGS){
            consecReadings = 0;
            return true;
        }
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
