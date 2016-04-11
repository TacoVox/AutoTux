//
// Created by niklas on 2016-04-08.
//

#include <iostream>
#include <thread>

#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

#include "packetio/PacketBroadcaster.h"
#include "decisionmaker/DecisionMaker.h"
#include "parker/Parker.h"

using namespace std;


using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace packetio;
using namespace decisionmaker;
using namespace parker;

enum STATE {DRIVING, SEARCHSFORSPACE, PARKING};

// takes the values of the argument passed to the constructor
int32_t ptrargc;
char** ptrargv;

// Shared pointer to the PcketBroadCaster
VehicleControl vehicleControl;
shared_ptr<Container> containerptr(new Container(vehicleControl));

/**
 * Constructor
 */
DecisionMaker::DecisionMaker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DecisionMaker") {
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

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DecisionMaker::body() {
    //The current state of the car.
    STATE state = DRIVING;

    //shared pointer to the Parker for the state of the car
    bool park;
    shared_ptr<bool> ptrParking(&park);

    /**
     * These creates shared pointers to be able to share data between them
     */
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(ptrargc, ptrargv));
    shared_ptr<Parker> parkerPointer(new Parker(ptrargc, ptrargv));

    /**
     * Starts the modules with the shared pointers
     */
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    thread parkerthread(&Parker::runModule, parkerPointer);

    packetBroadcaster->setControlDataContainer(containerptr);


    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        packetBroadcaster->setControlDataContainer(containerptr);

        if(state == DRIVING){
            cout << "Is now Driving" << endl;
        }
        if(state == SEARCHSFORSPACE){
            cout << "is now searching for spot" << endl;
        }
        if(state == PARKING){
            *ptrParking = true;
            parkerPointer->setParking(ptrParking);
        }

    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

/**
 * This method is a data listener that listens to the broadcast made by odsupercomponent
 *
 * @TODO Make it listen to the data sent from the camera instead of the VehicleControl container!
 */
void decisionmaker::DecisionMaker::nextContainer(odcore::data::Container &c) {
    if(c.getDataType() == VehicleControl::ID()){
        *containerptr = c; //Pointer to which the PacketBroadcaster sends for data.
    }
}

