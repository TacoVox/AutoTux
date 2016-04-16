//
// Created by niklas on 2016-04-08.
//

#include <iostream>
#include <thread>

#include "packetio/PacketBroadcaster.h"
#include "decisionmaker/DecisionMaker.h"
#include "parker/Parker.h"
#include "overtaker/Overtaker.h"

using namespace std;


using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace autotux;

using namespace packetio;
using namespace decisionmaker;
using namespace parker;
using namespace overtaker;

enum STATE {DRIVING, PARKING, TEST};

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

    // Parker shared pointers
    bool park;
    shared_ptr<bool> ptrParking(&park);

    VehicleControl parkcontrol;
    shared_ptr<Container> parkControlptr(new Container(parkcontrol));
    shared_ptr<Parker> parkerPointer(new Parker(ptrargc, ptrargv));

    //Set values in parker
    parkerPointer->setParking(ptrParking);
    parkerPointer->setParkingControler(parkControlptr);

    // Overtaker shared pointers
    VehicleControl ovtControl;
    shared_ptr<Container> ovtControlPtr (new Container(ovtControl));
    shared_ptr<Overtaker> ovtPtr (new Overtaker ());

    // Set container value in overtaker
    ovtPtr->setOvtControlDataContainer(ovtControlPtr);

    // PacketBroadcaster pointer
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(ptrargc, ptrargv));


    //Starts the modules with the shared pointers
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    //thread parkerthread(&Parker::runModule, parkerPointer);
    //thread ovtThread(&Overtaker::runModule, ovtPtr);

    packetBroadcaster->setControlDataContainer(containerptr);

    vehicleControl.setSpeed(1);
    *containerptr = vehicleControl;

    Container containerVehicleData;
    Container containerSensorBoardData;

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

                // Run obstacle-detection in overtaker...
                cout << "*******" << sbd.getValueForKey_MapOfDistances(3) << endl;
                ovtPtr->obstacleDetection(sbd, vd);


                if(ovtPtr->getIsOverriding()){
                    cout << "DRIVING: Overtaker is driving " << endl;
                    //*containerptr = *ovtControlPtr;
                }

                // ...else execute lane following instructions
                else{
                    cout << "DRIVING: Lane Follower is driving " << endl;
                    laneFollowing();
                }

                break;
            }
            case PARKING:{
                *ptrParking = true;

                if(parkerPointer->getFoundSpot()){
                    *containerptr = *parkControlptr;
                }
                else laneFollowing();
                break;
            }
            case TEST:{

                cout << "DM: Test-Mode" << endl;

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

