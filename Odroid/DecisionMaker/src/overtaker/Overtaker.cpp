//
// Created by marco on 2016-04-07.
//
#include <iostream>
#include "overtaker/Overtaker.h"

using namespace odcore::data;                 // Allows 'Container'

using namespace overtaker;

/* @doc Constructor.
 * */
Overtaker::Overtaker():
    ovtControlDataContainer(NULL) , isOverridingControls(false),
    state(FREE_LANE), traveledPath(0){}

Overtaker::~Overtaker(){}

/* @doc Run obstacle detection according to overtaker sub-state
 * */
void Overtaker::obstacleDetection(automotive::miniature::SensorBoardData sensorData, automotive::VehicleData vehicleData) {

    switch(state){
        case FREE_LANE:{
            cout << "Overtaker: run FREE-LANE obstacle detection" << endl;
            if(isObstacleDetected(sensorData, 15.0)){
                cout << "Overtaker: switching to APPROACHING state" << endl;
                state = APPROACHING;
            }
            break;
        }
        case APPROACHING:{
            cout << "Overtaker: run APPROACHING obstacle detection" << endl;
            if(isObstacleDetected(sensorData, 8.0)){
                cout << "Overtaker: switching to LEFT-SWITCH state" << endl;
                traveledPath = getTraveledPath(vehicleData);
                isOverridingControls = true;
                state = LEFT_SWITCH;
            }
            break;
        }
        case LEFT_SWITCH:{
            cout << "Overtaker: run LEFT-SWITCH obstacle detection and car control" << endl;

            if(switchToLeftLane(vehicleData, traveledPath, 2.0)){
                    isOverridingControls = false;
                    state = PARALLEL;
            }
            break;
        }
        case PARALLEL:{
            cout << "Overtaker: run PARALLEL obstacle detection" << endl;
            break;
        }
        case RIGHT_SWITCH:{
            cout << "Overtaker: run PARALLEL obstacle detection" << endl;
            break;
        }
    }

    // Set control values in shared pointer
    *ovtControlDataContainer = ovtVc;
}

/* @doc Setter for private Overtaker control data container
 * */
void Overtaker::setOvtControlDataContainer(std::shared_ptr<odcore::data::Container> container){
    this->ovtControlDataContainer = container;
}

/* @doc Getter for boolean value 'isOverridingControls'
 * */
bool Overtaker::getIsOverriding(){
    return isOverridingControls;
}

/* @doc Sets the traveled path to the value read from 'Sensor Board Data'
 * */
double Overtaker::getTraveledPath(automotive::VehicleData vehicleData){
    return vehicleData.getAbsTraveledPath();
}

/* @doc Reads the value of 'sensor' and compare it with 'limit'.
 *      Returns sensor value if value is within limit;
 *      returns -2 if sensor value is above limit
 * */
double Overtaker::sensorValueWithLimit(automotive::miniature::SensorBoardData sensorData,const double sensor, const double limit){

    double value = sensorData.getValueForKey_MapOfDistances(sensor);

    if(value <= limit){
        return value;
    }

    else return -2;     // sensor value is above limit

}

/* @doc Returns true 'sensor' detects an object within maxDistance
 * */
bool Overtaker::isObstacleDetected(automotive::miniature::SensorBoardData sensorData , const double maxDist) {

    double sensorVal = sensorData.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);

    // DEBUG** Remove this line
    cout << "Front US sensor : " << sensorVal << endl;

    if(sensorVal < maxDist && sensorVal > 0) return true;

    return false;
}

/* @doc Moves the car towards left lane for a distance
 *      of 'maxTrv' starting from 'trvPath'.
 * */
bool Overtaker::switchToLeftLane(automotive::VehicleData vehicleData, double trvPath, double maxTrv) {

    double path = vehicleData.getAbsTraveledPath() - trvPath;

    cout << "PATH: " << path << endl;

    // If maneuver is complete...
    if(path > maxTrv){
        return true;
    }

    // ... else keep driving in blind-mode
    ovtVc.setSteeringWheelAngle(-30);

    return false;
}

/* @doc Moves the car towards right lane for a distance
 *      of 'maxTrv' starting from 'trvPath'.
 * */
void Overtaker::switchToRightLane(automotive::VehicleData vehicleData, double trvPath, double maxTrv) {

    // Update distance traveled in 'blind' mode
    double path = vehicleData.getAbsTraveledPath() - trvPath;

    cout << "PATH: " << path << endl;

    // If traveled dist is above max-dist...
    if(path > maxTrv){
        isOverridingControls = false;
        state = FREE_LANE;
        return;
    }

    // ... else keep driving in blind-mode
    ovtVc.setSteeringWheelAngle(30);
}

/* @doc Returns TRUE if the right lane is free
 *      and car can return to right lane tp complete
 *      overtaking maneuver
 * */
bool Overtaker::isRightLaneClear(){
    // To-Do...

    return false;
}