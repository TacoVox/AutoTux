//
// Created by Jonas Kahler on 4/1/16.
//

#include "packetio/PacketReceiver.h"
#include <iostream>
#include <typeinfo>
#include <automotivedata/generated/automotive/VehicleControl.h>

using namespace std;
using namespace automotive;
using namespace odcore::base::module;
using namespace odcore::data;

packetio::PacketReceiver::PacketReceiver(const int32_t &argc, char **argv) :
    DataTriggeredConferenceClientModule(argc, argv, "AutoTuxProxy - PacketReceiver"),
    bufferWrapper(NULL) {
    cout << "Create PacketReceiver Object..." << endl;
}

packetio::PacketReceiver::~PacketReceiver() {}

void packetio::PacketReceiver::setUp() {
    cout << "PacketReveicer initialized [OK]" << endl;
}

void packetio::PacketReceiver::tearDown() {}

void packetio::PacketReceiver::nextContainer(Container &c) {
    //Check if valid ControlData//Guard
    if(c.getDataType() == 1) {
        cout << "Received ControlData" << endl;
        VehicleControl vehicleControl = c.getData<VehicleControl>();
        std::vector<unsigned char> data {'3', ':'};
        data.push_back((unsigned char)vehicleControl.getSpeed());
        data.push_back((unsigned char)vehicleControl.getSteeringWheelAngle());
        data.push_back(checksum(data));
        data.push_back(',');
        bufferWrapper->appendSendBuffer(data);
    } else {
        cout << "Received: " << c.getDataType() << endl;
        cout << "Received invalid data" << endl;
    }
}

void packetio::PacketReceiver::setBufferWrapper(
        std::shared_ptr<serial::BufferWrapper> bufferWrapper) {
    this->bufferWrapper = bufferWrapper;
    std::vector<unsigned char> data {'3', ':', 0, 90, 95, ','};
    bufferWrapper->appendSendBuffer(data);
}

unsigned char packetio::PacketReceiver::checksum(std::vector<unsigned char> v) {
    unsigned char cs = 0;
    for(auto it = v.begin(); it < v.end(); it++)
        cs ^= *it;
    return cs;
}

