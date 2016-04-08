//
// Created by Jonas Kahler on 4/1/16.
//

#include "packetio/PacketReceiver.h"
#include <iostream>
#include <automotivedata/generated/automotive/VehicleControl.h>

using namespace std;
using namespace automotive;
using namespace odcore::base::module;
using namespace odcore::data;

packetio::PacketReceiver::PacketReceiver(const int32_t &argc, char **argv) :
    DataTriggeredConferenceClientModule(argc, argv, "DataTriggeredReceiver") {}

packetio::PacketReceiver::~PacketReceiver() {}

void packetio::PacketReceiver::setUp() {}

void packetio::PacketReceiver::tearDown() {}

void packetio::PacketReceiver::nextContainer(Container &c) {
    //Check if valid ControlData//Guard
    if(c.getDataType() == VehicleControl::ID()) {
        cout << "Received ControlData" << endl;
    } else if (CHECKFORFLAG) {
        cout << "Received invalid data" << endl;
    }
}
