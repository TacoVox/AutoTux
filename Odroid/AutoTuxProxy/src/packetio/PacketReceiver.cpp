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
    DataTriggeredConferenceClientModule(argc, argv, "DataTriggeredReceiver") {
    cout << "Create PacketReceiver Object..." << endl;
}

packetio::PacketReceiver::~PacketReceiver() {}

void packetio::PacketReceiver::setUp() {}

void packetio::PacketReceiver::tearDown() {}

void packetio::PacketReceiver::nextContainer(Container &c) {
    //Check if valid ControlData//Guard
    if(c.getDataType() == VehicleControl::ID()) {
        cout << "Received ControlData" << endl;
        bufferWrapper->appendSendBuffer("5080");
    } else {
        cout << "Received invalid data" << endl;
    }
}

void packetio::PacketReceiver::setBufferWrapper(
        std::shared_ptr<serial::BufferWrapper> bufferWrapper) {
    this->bufferWrapper = bufferWrapper;
}
