//
// Created by Jonas Kahler on 4/1/16.
//

#include "../../include/packetio/PacketBroadcaster.h"

#include <iostream>

using namespace std;
using namespace odcore::base::module;
using namespace odcore::data;

packetio::PacketBroadcaster::PacketBroadcaster(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "TimeTriggeredSender"),
        sensorBoardDataContainer(NULL), interrupted(false) {
    cout << "Create PacketBroadcaster Object..." << endl;
}

packetio::PacketBroadcaster::~PacketBroadcaster() {}

void packetio::PacketBroadcaster::setUp() {
    cout << "Started the broadcasting component..." << endl;
}

void packetio::PacketBroadcaster::tearDown() {
    cout << "Stopped the broadcasting component..." << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode packetio::PacketBroadcaster::body() {
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
           odcore::data::dmcp::ModuleStateMessage::RUNNING && !interrupted) {
        cout << "Checking for vals" << endl;
        if(sensorBoardDataContainer != NULL) {
            cout << "Broadcasting values: ";
            cout << sensorBoardDataContainer->toString() << endl;
            getConference().send(*sensorBoardDataContainer);
        }
    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void packetio::PacketBroadcaster::setSensorBoardDataContainer(
        std::shared_ptr<odcore::data::Container> sensorBoardDataContainer) {
    cout << "Set SBDC..." << endl;
    //If there is data set - remove it from memory
    if(this->sensorBoardDataContainer != NULL) {
        this->sensorBoardDataContainer->~Container();
    }
    this->sensorBoardDataContainer = sensorBoardDataContainer;
}

