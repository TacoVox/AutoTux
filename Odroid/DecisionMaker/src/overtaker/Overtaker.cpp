//
// Created by marco on 2016-04-07.
//
#include <iostream>

#include "overtaker/Overtaker.h"

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

//using namespace odcore::base;
//using namespace odcore::base::module;
using namespace odcore::data;               // Allows 'Container'
//using namespace automotive::miniature;      // Allows 'Sensor Board Data'
//using namespace automotive;                 // Allows 'Vehicle Data'

using namespace overtaker;

/* @doc Constructor.
 * */
Overtaker::Overtaker(const int32_t &argc, char **argv):
    TimeTriggeredConferenceClientModule(argc, argv, "Overtaker"),
    ovtControlDataContainer(NULL) , isOverridingControls(false){}

Overtaker::~Overtaker(){}

void Overtaker::setUp() {
    cout << "Starting Overtaker component..." << endl;
}

void Overtaker::tearDown() {
    cout << "Terminating Overtaker component..." << endl;
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

/* @doc Reads the value of 'sensor' and compare it with 'limit'.
 *      Returns sensor value if value is within limit;
 *      returns -2 if sensor value is above limit
 * */
double Overtaker::sensorValueWithLimit(const double sensor, const double limit){

    double value = sbd.getValueForKey_MapOfDistances(sensor);

    if(value <= limit){
        return value;
    }

    else return -2;     // sensor value is above limit

}

/* @doc Returns true 'sensor' detects an object within maxDistance
 * */
bool Overtaker::isObstacleDetected(automotive::miniature::SensorBoardData sbd, const double maxDist) {

    double sensorVal = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_FORWARD);

    // DEBUG** Remove this line
    cout << "Front US sensor : " << sensorVal << endl;

    if(sensorVal < maxDist && sensorVal >= 0) return true;

    return false;
}

/* @doc Moves the car towards left lane for a distance
 *      of 'maxTrv' starting from 'trvPath'.
 * */
void Overtaker::switchToLeftLane(double trvPath, double maxTrv) {

    // Update distance traveled in 'blind' mode
    double path = vd.getAbsTraveledPath() - trvPath;

    cout << "PATH: " << path << endl;

    // If traveled dist is above max-dist...
    if(path > maxTrv){
        isOverridingControls = false;
        state = PARALLEL;
        return;
    }

    // ... else keep driving in blind-mode
    ovtVc.setSteeringWheelAngle(-30);
}

/* @doc Moves the car towards right lane for a distance
 *      of 'maxTrv' starting from 'trvPath'.
 * */
void Overtaker::switchToRightLane(double trvPath, double maxTrv) {

    // Update distance traveled in 'blind' mode
    double path = vd.getAbsTraveledPath() - trvPath;

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
    double us_fr = sensorValueWithLimit(ULTRASONIC_FRONT_RIGHT, 5);
    double ir_fr = sensorValueWithLimit(INFRARED_FRONT_RIGHT, 5);
    double ir_rr = sensorValueWithLimit(INFRARED_REAR_RIGHT, 5);

    cout << "US_FR : " << us_fr << endl;
    cout << "IR_FR : " << ir_fr << endl;
    cout << "IR_RR : " << ir_rr << endl;

    if(us_fr <= -1 && ir_fr <= -1 && ir_rr <= -1){
        return true;
    }

    return false;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Overtaker::body(){

    Container containerVehicleData;
    Container containerSensorBoardData;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING){

        // 1. Get most recent vehicle data:
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        // 2. Get most recent sensor board data describing virtual sensor data:
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        // Set initial speed in vehicle control
        ovtVc.setSpeed(1);

        switch(state){
            case FREE_LANE:{
                // Check front for obstacles
                if(isObstacleDetected(sbd, 15.0)){
                    cout << "Overtaker: Obstacle detected" << endl;
                    state = APPROACHING;
                }
                break;
            }
            case APPROACHING:{
                // Check min distance
                if(isObstacleDetected(sbd, 10.0)) {
                    cout << "Overtaker: Initiating Left-Switch maneuver" << endl;
                    traveledPath = vd.getAbsTraveledPath();     // Store traveled path before left switch takes over
                    state = LEFT_SWITCH;
                }
                break;
            }
            case LEFT_SWITCH:{
                // Set vehicle control
                cout << "Overtaker: Switching left maneuver" << endl;
                isOverridingControls = true;
                switchToLeftLane(traveledPath, 3.0);
                break;
            }
            case PARALLEL:{
                cout << "Overtaker: is parallel to obstacle" << endl;

                // Check right-side sensors
                if(isRightLaneClear()){
                    cout << "Overtaker: initiating right switch maneuver"<< endl;
                    traveledPath = vd.getAbsTraveledPath();
                    state = RIGHT_SWITCH;
                }
                break;
            }
            case RIGHT_SWITCH:{
                cout << "Overtaker: Switching right maneuver" << endl;
                isOverridingControls = true;
                switchToRightLane(traveledPath, 3.0);
                break;
            }
        }

        // Set control values in shared pointer
        *ovtControlDataContainer = ovtVc;
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}