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
        controlDataContainer(NULL), interrupted(false) {}

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
        if(controlDataContainer != NULL) {
            cout << "Broadcasting values..." << endl;
            getConference().send(*controlDataContainer);
        }
    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void packetio::PacketBroadcaster::setControlDataContainer(
        std::shared_ptr<odcore::data::Container> controlDataContainer) {
    this->controlDataContainer = controlDataContainer;
}

