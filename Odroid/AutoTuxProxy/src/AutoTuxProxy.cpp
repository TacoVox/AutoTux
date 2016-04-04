//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <memory>

#include <opendavinci/odcore/data/Container.h>

#include "../include/packetio/PacketBroadcaster.h"
#include "serial/USBConnector.h"

using namespace std;
using namespace odcore::data;

int main(int argc, char **argv) {
    cout << "Testing Packet Broadcaster!" << endl;
    packetio::PacketBroadcaster packetBroadcaster(argc, argv);
    packetBroadcaster;
    packetBroadcaster.runModule();

    //Create pointer
    Container *container;
    //Set Broadcaster pointer
    //packetBroadcaster.setSensorBoardDataContainer(container);

    cout << "Testing USBConnector!" << endl;
    USBConnector serial_obj;
    serial_obj.connect();
    serial_obj.read();
    serial_obj.disconnect();

    return 0;
}