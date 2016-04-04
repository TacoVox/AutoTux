//
// Created by jonas on 4/1/16.
//
#include <iostream>

#include "../include/packetio/PacketBroadcaster.h"
#include "serial/USBConnector.h"

using namespace std;

int main() {
    cout << "Testing Packet Broadcaster!" << endl;
    packetio::PacketBroadcaster packetBroadcaster(argc, argv);
    packetBroadcaster;
    packetBroadcaster.runModule();

    cout << "Testing USBConnector!" << endl;
    USBConnector serial_obj;
    serial_obj.connect();
    serial_obj.read();
    serial_obj.disconnect();

    return 0;
}