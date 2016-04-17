//
// Created by niklas on 2016-04-08.
//

#include <iostream>
#include <string>
#include <thread>

#include "packetio/PacketBroadcaster.h"
#include "decisionmaker/DecisionMaker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;               // Container
using namespace automotive;                 // Vehicle Control & Vehicle Data
using namespace automotive::miniature;      // Sensor Board Data

using namespace autotux;

using namespace packetio;
using namespace decisionmaker;

enum STATE {DRIVING, PARKING};

// takes the values of the argument passed to the constructor
int32_t ptrargc;
char** ptrargv;

// Shared pointer to Container
VehicleControl vehicleControl;
shared_ptr<Container> containerptr(new Container(vehicleControl));

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker"),
        laneRecommendation() {
    ptrargc = argc;
    ptrargv = argv;
}

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
    vehicleControl.setSteeringWheelAngle(getAngle());
    *containerptr = vehicleControl;
}

/**
 * Get the angle given by LaneRecommandation
 */
double DecisionMaker::getAngle() {
    return laneRecommendation.getData<LaneRecommendation>().getAngle();
}
/**
 * Get's a bool if the car is in leftlane
 * @TODO This isn't working yet
 */
bool DecisionMaker::isInLeftLane() {
    return laneRecommendation.getData<LaneRecommendation>().getLeft_lane();

}
/**
 * How good the Data sent by the LaneRecommendation are.
 */
bool DecisionMaker::isDataQuality(){
    return laneRecommendation.getData<LaneRecommendation>().getQuality();

}
/**
 * Get's the distance from a stop Line
 * @TODO is not found yet
 */
double DecisionMaker::getDistanceToLine() {
    return laneRecommendation.getData<LaneRecommendation>().getDistance_to_line();

}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DecisionMaker::body() {

    // Set initial state of the car
    STATE state = DRIVING;

    // PacketBroadcaster pointer
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(ptrargc, ptrargv));

    //Starts the modules with the shared pointers
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);

    // Set container pointer in packet broadcaster
    packetBroadcaster->setControlDataContainer(containerptr);

    Container containerVehicleData;
    Container containerSensorBoardData;

    VehicleData vd;
    SensorBoardData sbd;

    // Set initial speed
    vehicleControl.setSpeed(2);
    *containerptr = vehicleControl;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        // Set Control Data Container into Packet Broadcaster
        packetBroadcaster->setControlDataContainer(containerptr);

        // 1. Get most recent vehicle data
        containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
        vd = containerVehicleData.getData<VehicleData>();

        // 2. Get most recent sensor board data
        containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
        sbd = containerSensorBoardData.getData<SensorBoardData>();

        switch (state){
            case DRIVING:{

                // To-Do
                // Run obstacle-detection in overtaker...

                laneFollowing();
                //*containerptr = vehicleControl;

                break;
            }
        }
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

/**
 * This method is a data listener that listens to the broadcast made by odsupercomponent for LaneRecommendation
*/
void decisionmaker::DecisionMaker::nextContainer(odcore::data::Container &c) {

    if(c.getDataType() == LaneRecommendation::ID()){
        laneRecommendation = c; //Pointer to which the PacketBroadcaster sends for data.
    }

}

