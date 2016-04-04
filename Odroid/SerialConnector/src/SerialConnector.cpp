//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <thread>
#include "../include/packetio/PacketBroadcaster.h"

using namespace std;

int main(int32_t argc, char **argv) {
    cout << "Testing Packet Broadcaster!" << endl;
    packetio::PacketBroadcaster packetBroadcaster(argc, argv);
    packetBroadcaster;
    return packetBroadcaster.runModule();

    //return 0;
}