//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <thread>
#include "packetio/PacketBroadcaster.h"
#include "packetio/PacketReceiver.h"

using namespace std;

int main() {
    cout << "Testing UDP Session!" << endl;
    packetio::UDPBroadcaster udpb;
    packetio::PacketReceiver packetReceiver;

    thread sendthread(&packetio::UDPBroadcaster::sendtest, udpb);
    thread receivethread(&packetio::PacketReceiver::test, packetReceiver);

    sendthread.join();
    receivethread.join();

    return 0;
}