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
    ovtControl(), isOverridingControls(false), traveledPath(0), totalTraveled(0), state(FREE_LANE),
    leftLane(false), consecReadings(0), idle_frame_counter(0), average_counter(0), min_us_fr(OVT_TRIGGER), laneFollowerAngle(0.0), laneRecommendation(0.0){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker Finite State Machine
 * */
void Overtaker::obstacleDetection(SensorBoardData sensorData, VehicleData vehicleData) {

    // Check if we want to reset FSM
    resetFSM(vehicleData, totalTraveled, 125);

    switch(state){
        case FREE_LANE:{
            // If close to obstacle switch lane to overtake...
            if(isObstacleOnLane(sensorData, OVT_TRIGGER)){
                cout << "Transition to INIT-LEFT-SWITCH" << endl;
                isOverridingControls = true;
                traveledPath = vehicleData.getAbsTraveledPath();
                min_us_fr = 0.4;
                ovtControl.setSpeed(1);
                ovtControl.setFlashingLightsLeft(true);
                state = INIT_LEFT_SWITCH;
            }
            break;
        }
	case INIT_LEFT_SWITCH:{
	    if(turnLeft(vehicleData, traveledPath, 0.25)){
	        cout << "Transition to LEFT-SWITCH" << endl;
		    traveledPath = vehicleData.getAbsTraveledPath();
		    state = LEFT_SWITCH;
	    }
	    break;
	}
        case LEFT_SWITCH:{
            if(cornerDetection(sensorData, vehicleData, ULTRASONIC_FRONT_RIGHT, traveledPath, LEFT_SWITCH_DIST)){
                cout << "Transition to ADJUST LEFT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsLeft(false);
                state = ADJUST_LEFT_SWITCH;
            }
            break;
        }
        case ADJUST_LEFT_SWITCH:{
            if(adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_DIST)){
                cout << "Transition to SET-PARALLEL" << endl;
                state = SEARCH_END;
            }
            break;
        }
        case SEARCH_END:{
            keepParallelToObstacle(sensorData, 0.05);
            if(detectEndOfObstacle(sensorData)){
                cout << "Transition to REACH-END" << endl;
                state = REACH_END;
            }
            break;
        }
        case REACH_END:{
            keepParallelToObstacle(sensorData, 0.05);
            if(isRightLaneClear(sensorData)){
                cout << "Transition to RIGHT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsRight(true);
                state = RIGHT_SWITCH;
            }
            break;
        }
        case RIGHT_SWITCH:{
            if(switchToRightLane(vehicleData, 0.5235, traveledPath, RIGHT_SWITCH_DIST)){
                cout << "Transition to ADJUST-RIGHT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsRight(false);
                state = ADJUST_RIGHT_SWITCH;
            }
            break;
        }
        case ADJUST_RIGHT_SWITCH:{
            if(adjustRightSwitch(vehicleData, traveledPath, ADJUST_R_S_DIST)){
                cout << "Transition to FREE-LANE" << endl;
                isOverridingControls = false;
                state = FREE_LANE;
            }
            break;
        }
    }
}

void Overtaker::newObstacleDetection(automotive::miniature::SensorBoardData sensorData, automotive::VehicleData vehicleData) {
    // Check if we want to reset FSM
    resetFSM(vehicleData, totalTraveled, 125);

    switch(state){
        case FREE_LANE:{
            // If close to obstacle switch lane to overtake...
            if(isObstacleOnLane(sensorData, OVT_TRIGGER)){
                cout << "Transition to INIT-LEFT-SWITCH" << endl;
                isOverridingControls = true;
                traveledPath = vehicleData.getAbsTraveledPath();
                min_us_fr = 0.4;
                ovtControl.setSpeed(1);
                ovtControl.setFlashingLightsLeft(true);
                state = INIT_LEFT_SWITCH;
            }
            break;
        }
        case INIT_LEFT_SWITCH:{
            if(turnLeft(vehicleData, traveledPath, 0.35)){
                cout << "Transition to LEFT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                state = LEFT_SWITCH;
            }
            break;
        }
        case LEFT_SWITCH:{
            if(cornerDetection(sensorData, vehicleData, ULTRASONIC_FRONT_RIGHT, traveledPath, LEFT_SWITCH_DIST)){
                cout << "Transition to ADJUST LEFT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsLeft(false);
                leftLane = true;
                state = ADJUST_LEFT_SWITCH;
            }
            break;
        }
        case ADJUST_LEFT_SWITCH:{
            if(adjustLeftSwitch(vehicleData, traveledPath, ADJUST_L_S_DIST)){
                cout << "Transition to SET-PARALLEL" << endl;
                isOverridingControls = false;
                state = SEARCH_END;
            }
            break;
        }
        case SEARCH_END:{
            if(detectEndOfObstacle(sensorData)){
                cout << "Transition to REACH-END" << endl;
                state = REACH_END;
            }
            break;
        }
        case REACH_END:{
            if(isRightLaneClear(sensorData)){
                cout << "Transition to RIGHT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsRight(true);
                isOverridingControls = true;

                // *** To-Do... : Adjust 'RIGHT_SWITCH_DIST' according to Lane follower instructions
                // cout << "***OVERTAKER: Lane Follower angle : " << laneFollowerAngle << endl;
		laneRecommendation = laneFollowerAngle;
                state = COMPUTE_ANGLE;
            }
            break;
        }
	case COMPUTE_ANGLE:{
		sumTotalAngle(laneFollowerAngle);
		if(average_counter >= 5){
			laneRecommendation /= 5;
			cout << "****AVERAGE ANGLE : " << laneRecommendation << endl;
			state = RIGHT_SWITCH;
		}
	}	
        case RIGHT_SWITCH:{
		double distanceToCover;
		double steeringAngle;
	if(laneRecommendation > 0.1){
		cout << "%% Return on RIGHT TURN" << endl;
		distanceToCover = RIGHT_SWITCH_DIST + 0.20;
		steeringAngle = 0.5235;
	} else if (laneRecommendation < -0.13){
		cout << "%% Return on LEFT TURN" << endl;
		distanceToCover = RIGHT_SWITCH_DIST - 0.15;
		steeringAngle = 0.30;
	}else{
		cout << "%% Return on STRAIGHT LANE" << endl;
		distanceToCover = RIGHT_SWITCH_DIST;
		steeringAngle = 0.5;
	}

            if(switchToRightLane(vehicleData, steeringAngle, traveledPath, distanceToCover)){
                cout << "Transition to ADJUST-RIGHT-SWITCH" << endl;
                traveledPath = vehicleData.getAbsTraveledPath();
                ovtControl.setFlashingLightsRight(false);
		state = ADJUST_RIGHT_SWITCH;
            }
            break;
        }
        case ADJUST_RIGHT_SWITCH:{
		double d = 0;
		if(laneRecommendation < -0.13){
			d = ADJUST_R_S_DIST;
		}
            if(adjustRightSwitch(vehicleData, traveledPath, d)){
                cout << "Transition to FREE-LANE" << endl;
                isOverridingControls = false;
                average_counter = 0;
		laneRecommendation = 0;
		leftLane = false;
                state = FREE_LANE;
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

/* @doc Getter for private field 'ovtControl'
 * */
VehicleControl Overtaker::getOvtControl() {
    return ovtControl;
}

void Overtaker::setLaneFollowerAngle(double val) {
    laneFollowerAngle = val;
}

/* @doc Helper function for testing. Stops the car.
 * */
void Overtaker::stopCar(){
    ovtControl.setSteeringWheelAngle(0.0);
    ovtControl.setSpeed(0);
}

/* @doc Helper function for testing. Resets Overtaker FSM
 *      .
 * */
void Overtaker::resetFSM(VehicleData vd, const double lastTraveled, const int maxFrames) {

    double newTraveled = vd.getAbsTraveledPath();

    if(newTraveled == lastTraveled){
        idle_frame_counter++;
        if(idle_frame_counter >= maxFrames){
            cout << "*** Resetting Overtaker FSM after " << maxFrames << " frames" << endl;
            idle_frame_counter = 0;
            ovtControl.setSteeringWheelAngle(0.0);
            state = FREE_LANE;
        }
    }else{
        idle_frame_counter = 0;
    }

    totalTraveled = newTraveled;

}


/* @doc Returns true if US_FF sensor detects obstacle in front of the car
 *      within 'range' distance.
 * */
bool Overtaker::isObstacleOnLane(SensorBoardData sbd, const double range){

    bool us_ff = isObstacleDetected(sbd, ULTRASONIC_FRONT_FORWARD, range);

    if(us_ff){
        cout << "*****FRONT US sensor: " << sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) << endl;
        if(checkReadingsCounter()) {
            return true;
        }
    }

    return false;
}

/* ...
*/
bool Overtaker::turnLeft(VehicleData vd, const double trvStart, const double distance) {
    double traveled = vd.getAbsTraveledPath() - trvStart;

    if(traveled > distance) {
        return true;
    }
	
    ovtControl.setSteeringWheelAngle(-0.5235);
    
    return false;
}
/* @doc Returns true if 'sensor' distance signals we passed a corner
 *      obstacle.
 * */
bool Overtaker::cornerDetection(SensorBoardData sbd, VehicleData vehicleData, const int sensor, const double trvStart, const double maxDistance){

    double frontRightUsSensor = sbd.getValueForKey_MapOfDistances(sensor);

    // Update minimum distance if corner was not spetted...
    if(frontRightUsSensor <= min_us_fr && frontRightUsSensor > 0){
        cout << "*** CORNER DETECTION: Min distance from" << min_us_fr << " to : " << frontRightUsSensor << endl;
        min_us_fr = frontRightUsSensor;
    }
    // ... else obstacle rear-left corner was passed by the car...
    else if(frontRightUsSensor > 0 && frontRightUsSensor > min_us_fr + 0.05){
        // Measure path traveled in 'blind-mode'
        double traveled = vehicleData.getAbsTraveledPath() - trvStart;
        consecReadings++;
        if(consecReadings > NUM_OF_READINGS && traveled > maxDistance){
            cout << "*** CORNER DETECTION - traveled : " << traveled << " - THRESHOLD: "<< maxDistance << endl;
            consecReadings = 0;
            return true;
        }
    } 

    // ... else keep steering left
    ovtControl.setSteeringWheelAngle(-0.5235);

    return false;
}

/* @doc Adjusts left switch maneuver by steering right and
 *      traveling 'maxTrv' distance from 'trvStart'.
 * */
bool Overtaker::adjustLeftSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // To-Do...
    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Reset steering angle and return true if traveled above limit...
    if(traveled > maxTrv){
        cout << "ADJUST-LEFT: traveled " << traveled << " - Threshold: " << maxTrv << endl;
 	    ovtControl.setSteeringWheelAngle(0.0);
        return true ;
    }

    //... else keep steering right
    ovtControl.setSteeringWheelAngle(0.5235);

    return false;
}

/* @doc Detects end of parallel obstacle on right side
 * */
bool Overtaker::detectEndOfObstacle(SensorBoardData sensorData) {

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, US_FRONT_RIGHT_RANGE);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, IR_SENSOR_LIMIT);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, IR_SENSOR_LIMIT);

    // If US_front-right detects a gap while right-side infrared detect the obstacle...
    if(!us_fr && ir_fr && ir_rr){
        cout << "END OF OBSTACLE detected..." << endl;

        if(checkReadingsCounter()) {
            return true;
        }
    }

    return false;
}

/* @doc Moves the car towards right lane for a distance
 *      of 'maxTrv' starting from 'trvStart'.
 * */
bool Overtaker::switchToRightLane(VehicleData vehicleData, const double angle, double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Exit state if traveled enough...
    if(traveled > maxTrv){
        cout << "SWITCH TO RIGHT LANE - traveled :" << traveled << " - THRESHOLD: " << maxTrv << endl;
        return true;
    }

    // ... else keep steering right
    ovtControl.setSteeringWheelAngle(angle);
    return false;
}

/* @doc Adjusts right switch maneuver by steering left and
 *      traveling 'maxTrv' distance from 'trvStart'.
 * */
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

/* @doc Returns TRUE if the right lane is free and car can
 *      return to right lane tp complete overtaking maneuver.
 * */
bool Overtaker::isRightLaneClear(SensorBoardData sensorData){

    // Check if right-side sensors detect obstacle
    bool us_fr = isObstacleDetected(sensorData, ULTRASONIC_FRONT_RIGHT, US_SENSOR_RANGE);
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, IR_SENSOR_LIMIT);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, IR_SENSOR_LIMIT);

    //cout << "IS RIGHT LANE CLEAR: US_FR : " << us_fr << " - IR_FR : " << ir_fr << " - IR_RR : " << ir_rr << endl;

    // If none of the right side sensors detect an obstacle...
    if(!us_fr && !ir_fr && !ir_rr){
        if(checkReadingsCounter()) {
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

/* @doc Keeps car parallel to obstacle on right lane
 * */
void Overtaker::keepParallelToObstacle(SensorBoardData sensorData, const double diff_margin) {
    bool ir_fr = isObstacleDetected(sensorData, INFRARED_FRONT_RIGHT, IR_SENSOR_LIMIT);
    bool ir_rr = isObstacleDetected(sensorData, INFRARED_REAR_RIGHT, IR_SENSOR_LIMIT);

    // If none of the two IR sensors is detecting the obstacle...
    if(!ir_fr && !ir_rr){
        cout << "KEEP-PARALLEL: Lost sight of obstacle" << endl;
        //ovtControl.setSteeringWheelAngle(0.5235);
        if(state!= REACH_END) {
            stopCar();
        }
        return;
    }

    // ...else if one of the two IR sensors do not detect anything...
    if(!ir_fr || !ir_rr ){
        return;
    }

    // ...else both IR sensors are detecting obstacle
    double frontRightDist = sensorData.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
    double rearRightDist = sensorData.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);

    double diff = frontRightDist - rearRightDist;

    // *** USE PROPORTIONAL GAIN?? ***
    cout << "***DIFF: " << diff << endl;
    // *** To-Do: adjust steering angle by difference
    if(diff > diff_margin){
        ovtControl.setSteeringWheelAngle(0.2);
        return;
    }

    if(diff < -diff_margin){
        ovtControl.setSteeringWheelAngle(-0.2);
        return;
    }
    ovtControl.setSteeringWheelAngle(0.0);
    return;
}

/* @doc Updates readings counter and checks if above limit.
 *      Resets the counter and returns TRUE if above limit, FALSE otherwise.
 * */
bool Overtaker::checkReadingsCounter() {

    consecReadings++;

    if(consecReadings >= NUM_OF_READINGS){

        consecReadings = 0;
        return true;
    }

    return false;
}

/* @doc Getter for private field leftLane
 * */
bool Overtaker::isLeftLane() {
    return leftLane;
}


void Overtaker::sumTotalAngle(double val){
	average_counter++;
	laneRecommendation += val;	
}
