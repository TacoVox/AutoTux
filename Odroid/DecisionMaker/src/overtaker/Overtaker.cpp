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
void Overtaker::obstacleDetection(automotive::miniature::SensorBoardData sensorData, automotive::VehicleData vehicleData) {
    // Check if we want to reset FSM
    resetFSM(vehicleData, totalTraveled, 125);

    switch(state){
        case FREE_LANE:{
            // If close to obstacle switch lane to overtake...
            if(isObstacleOnLane(sensorData, OVT_TRIGGER)){
                cout << "Transition to INIT-LEFT-SWITCH" << endl;
                isOverridingControls = true;
                traveledPath = vehicleData.getAbsTraveledPath();
                min_us_fr = OVT_TRIGGER;
                ovtControl.setSpeed(1);
                ovtControl.setFlashingLightsLeft(true);
                state = INIT_LEFT_SWITCH;
            }
            break;
        }
        case INIT_LEFT_SWITCH:{
            if(turnLeft(vehicleData, traveledPath, INIT_LS_DIST)){
                cout << "Transition to LEFT-SWITCH" << endl;
                //traveledPath = vehicleData.getAbsTraveledPath();
                state = LEFT_SWITCH;
            }
            break;
        }
        case LEFT_SWITCH:{
            if(cornerDetection(sensorData, vehicleData, ULTRASONIC_FRONT_RIGHT)){
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
                isOverridingControls = false;
                leftLane = true;
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
                state = COMPUTE_ANGLE;
            }
            break;
        }
        case COMPUTE_ANGLE:{
            sumTotalAngle(laneFollowerAngle);
            if(average_counter >= 5){
                laneRecommendation /= 5;
                cout << "****AVERAGE ANGLE : " << laneRecommendation << endl;
                isOverridingControls = true;
                state = RIGHT_SWITCH;
            }
            break;
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

            // If maneuver was completed, switch to next state...
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

            // If adjust maneuver was completed, switch to free-lane...
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

/* @doc Setter for private field 'laneFollowerAngle'
 * */
void Overtaker::setLaneFollowerAngle(double val) {
    laneFollowerAngle = val;
}

/* @doc Helper function for testing. Stops the car.
 * */
void Overtaker::stopCar(){
    ovtControl.setSteeringWheelAngle(0.0);
    ovtControl.setSpeed(0);
}

/* @doc Helper function for testing. Resets Overtaker FSM if car has not traveled over
 *      the past 'maxFrames'.
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

/* @doc Returns true if US Front-Forward sensor detects an obstacle in front of the car
 *      within 'range' distance for 'NUM_OF_READINGS' consecutive frames.
 * */
bool Overtaker::isObstacleOnLane(SensorBoardData sbd, const double range){

    bool us_ff = isObstacleDetected(sbd, ULTRASONIC_FRONT_FORWARD, range);

    // If US_FF detects obstacle within range...
    if(us_ff){
        cout << "*****FRONT US sensor: " << sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD) << endl;
        if(checkReadingsCounter()) {
            return true;
        }
    }
    //... else reset consecutive readings counter
    else{
        resetReadingsCounter();
    }

    return false;
}

/* @doc Returns true after traveling a path of length 'distance' from trvStart
 *      while steering 30° to the left. Returns false if covered distance is less
 *      or equal to 'distance'.
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
bool Overtaker::cornerDetection(SensorBoardData sbd, VehicleData vehicleData, const int sensor){

    double frontRightUsSensor = sbd.getValueForKey_MapOfDistances(sensor);

    // Update minimum distance if corner was not spetted...
    if(frontRightUsSensor <= min_us_fr && frontRightUsSensor > 0){
        cout << "*** CORNER DETECTION: Min distance from" << min_us_fr << " to : " << frontRightUsSensor << endl;
        min_us_fr = frontRightUsSensor;
    }
    // ... else obstacle rear-left corner was passed by the car...
    else if(frontRightUsSensor > 0 && frontRightUsSensor > min_us_fr + CORNER_DET_MARGIN){

        if(checkReadingsCounter()){
            return true;
        }
    }
    //...else reset consecutive readings counter
    else{
        consecReadings = 0;
    }

    // ... else keep steering left
    ovtControl.setSteeringWheelAngle(-0.5235);

    return false;
}

/* @doc Adjusts left switch maneuver by steering right and
 *      traveling 'maxTrv' distance from 'trvStart'. Returns true after maneuver is completed.
 * */
bool Overtaker::adjustLeftSwitch(VehicleData vehicleData, const double trvStart, const double maxTrv) {

    // Measure path traveled in 'blind-mode'
    double traveled = vehicleData.getAbsTraveledPath() - trvStart;

    // Reset steering angle and return true if traveled above limit...
    if(traveled > maxTrv){
        cout << "ADJUST-LEFT: traveled " << traveled << " - Threshold: " << maxTrv << endl;
 	    ovtControl.setSteeringWheelAngle(0.0);
        return true ;
    }

    //... else keep steering right
    ovtControl.setSteeringWheelAngle(0.2);

    return false;
}

/* @doc Returns true after end of parallel obstacle on right side is detected
 *      for 'NUM_OF_READINGS' consecutive frames.
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
    //... else reset consecutive readings counter
    else{
        resetReadingsCounter();
    }

    return false;
}

/* @doc Moves the car towards right lane traveling a path of length 'maxTrv'
 *      starting from 'trvStart'. Returns true when maneuver is completed.
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

/* @doc Adjusts right switch maneuver by steering left and traveling 'maxTrv'
 *      distance from 'trvStart'. Returns true when maneuver is completed.
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

/* @doc Returns TRUE if the right lane is detected as free
 *      for 'NUM_OF_READINGS' consecutive frames.
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
    //... else reset consecutive readings counter
    else{
        resetReadingsCounter();
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

/* @doc Resets consecutive readings counter to zero.
 * */
void Overtaker::resetReadingsCounter() {
    consecReadings = 0;
}

/* @doc Getter for private field leftLane
 * */
bool Overtaker::isLeftLane() {
    return leftLane;
}

/* @doc Helper function for computing average lane angle in COMPUTE_ANGLE state.
 * */
void Overtaker::sumTotalAngle(double val){
	average_counter++;
	laneRecommendation += val;	
}
