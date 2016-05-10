//
// Created by niklas on 2016-04-08.
//

#include <iostream>

#include "decisionmaker/DecisionMaker.h"
#include "overtaker/Overtaker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;               // Container
using namespace automotive;                 // Vehicle Control & Vehicle Data
using namespace automotive::miniature;      // Sensor Board Data
using namespace automotive::vehicle;        // For reversing light

using namespace autotux;                    // For own data structures

using namespace decisionmaker;
using namespace overtaker;


VehicleControl vehicleControl;

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker"),
        state(LANE_FOLLOWING),ovt(), parker(), vd(), sbd(), dmMSG(), lrMSG(),
        speed(), isStopLine(false), stopCounter(0), printCounter(0) {}

DecisionMaker::~DecisionMaker() {}

void DecisionMaker::setUp(){
    cout << "DecisionMaker is setting up" << endl;
}
void DecisionMaker::tearDown(){
    cout << "DecisionMaker shuts down" << endl;
}
/**
 * Sets wheelangledata to the LaneRecommandation
*/
void DecisionMaker::laneFollowing() {

    if(stopCounter > 0) {

        if(stopCounter == 50) {
            //cout << "WAKING UP" << endl;
            stopCounter = 0;
            isStopLine = false;
            vehicleControl.setBrakeLights(false);
        }

        else {
            //cout << "SLEEPING..." << endl;
            stopCounter++;
        }
    }

    else if(getDistanceToLine() < 30 && getDistanceToLine() != -1) {
        //cout << "STOPPING!" << endl;
        //vehicleControl.setBrakeLights(true);
        //speed = 0;
        //stopCounter = 1;
        //isStopLine = true;
    }

    else if(getDistanceToLine() < 50 && getDistanceToLine() != -1) {
        //cout << "Slowing down..." << endl;
        vehicleControl.setBrakeLights(false);
        speed = 1;
    }

    vehicleControl.setSpeed(speed);
    vehicleControl.setSteeringWheelAngle(getAngle());
}

/**
 * Gets the angle given by LaneRecommandation
 */
double DecisionMaker::getAngle() {
    return lrMSG.getAngle();
}
/**
 * True if Data sent by the LaneRecommendation are considered reliable.
 */
bool DecisionMaker::isDataQuality(){
    return lrMSG.getQuality();
}
/**
 * Get's the distance from a stop Line
 * @TODO is not found yet
 */
double DecisionMaker::getDistanceToLine() {
    return lrMSG.getDistance_to_line();
}

/**
 * Function that prints debug output every second instead of every iterration.
 */
void DecisionMaker::printDebug() {
    if(printCounter == 30) {

        // Values received from lane follower
        cout << "STOPLINE: " << getDistanceToLine() << endl;
        cout << "DESIRED STEERING: " << getAngle() << endl;

        // Sensors received from proxy
        cout << "IR FRONT RIGHT: " << sbd.getValueForKey_MapOfDistances(0);
        cout << " | IR REAR: " << sbd.getValueForKey_MapOfDistances(1);
        cout << " | IR REAR RIGHT: " << sbd.getValueForKey_MapOfDistances(2);
        cout << " | US FRONT: " << sbd.getValueForKey_MapOfDistances(3);
        cout << " | US FRONT RIGHT: " << sbd.getValueForKey_MapOfDistances(4);
        cout << " | TRAVELED: " << vd.getAbsTraveledPath() << endl;
	
	printf("%i\n", state);

        // Reset counter
        printCounter = 0;
    }
    else {
        printCounter++;
    }
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DecisionMaker::body() {
    LIFOQueue lifoQueue;
    addDataStoreFor(lifoQueue);

    Container containerSensorBoardData, containerVehicleData, containerDecisionMakerMSG, containerLaneRecommendationMSG;
    OvertakingMSG ovtMSG;
    LightSystem lightSystem;


    // Set initial speed
    vehicleControl.setSpeed(2.0);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        // 1. Update sensor board data values
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        // 2. Update vehicle data values
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        containerDecisionMakerMSG = getKeyValueDataStore().get(autotux::DecisionMakerMSG::ID());
        dmMSG = containerDecisionMakerMSG.getData<DecisionMakerMSG>();

        containerLaneRecommendationMSG = getKeyValueDataStore().get(autotux::LaneRecommendationMSG::ID());
        lrMSG = containerLaneRecommendationMSG.getData<LaneRecommendationMSG>();

      	state = static_cast<DecisionMaker::STATE>(dmMSG.getState());
	
        if(!ovt.isLeftLane()){
            ovtMSG.setLeftlane(NOTLEFTLANE);
        }
        else
            ovtMSG.setLeftlane(LEFTLANE);

        switch (state){
            // Pure Lane follower
            case LANE_FOLLOWING:{

                if(!isStopLine) {
                    speed = 2;
                }

                laneFollowing();

                break;
            }
            // Lane follower + overtaker
            case DRIVING:{
                ovt.obstacleDetection(sbd, vd, vehicleControl);

                // If overtaker is overriding control values...
                if(ovt.getIsOverriding()) {
                    //cout << "DM: OVERTAKER is OVERRIDING" << endl;
                    vehicleControl = ovt.getOvtControl();

				}
                //... else follow lane-follower instructions...
                else{
                    //cout <<"DM: LANE FOLLOWER Instructions" << endl;

                    if(!isStopLine) {
                        speed = 1;
                    }

                    laneFollowing();
                }

                break;
            }
            // Lane follower + parking
            case PARKING:{
                if(parker.getFoundSpot()){
                    if(parker.isReversing()){
                        lightSystem.setReverseLight(true);
                    }
                    else
                        lightSystem.setReverseLight(false);

                    if(!parker.getIsParked()) {
                        vehicleControl = parker.parallelPark(sbd, vd);
                    }
		break;
                }
                else{
                    if(!isStopLine) {
                        parker.findSpot(sbd, vd, vehicleControl);
                        speed = 1;
                    }
                    laneFollowing();
                }
                break;
            }
            case RESUME:{
                if(!parker.isOutOfSpot()){
                    vehicleControl = parker.goBackToLane(sbd, vd, 0.1);
                }
                else
                    state = LANE_FOLLOWING;
                break;
            }
        }

        // Print debug output
        printDebug();

        // Pack and send containers
        Container control(vehicleControl);
        Container lane(ovtMSG);
        Container lights(lightSystem);
        getConference().send(control);
        getConference().send(lane);
        getConference().send(lights);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
