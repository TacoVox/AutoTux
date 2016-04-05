//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <thread>

#include <opendavinci/odcore/data/Container.h>

#include "packetio/PacketBroadcaster.h"
#include "containerfactory/SBDContainer.h"
#include "serial/USBConnector.h"

using namespace std;
using namespace odcore::data;

int main(int argc, char **argv) {
    cout << "Testing Packet Broadcaster!" << endl;

    std::shared_ptr<packetio::PacketBroadcaster> packetBroadcaster(new packetio::PacketBroadcaster(argc, argv));
    std::thread pbthread(&packetio::PacketBroadcaster::runModule, packetBroadcaster);

    //cout << "Testing USBConnector!" << endl;
    //USBConnector serial_obj;
    //serial_obj.connect();
    //serial_obj.read();

    //Here we have to start reading and then wrapping the shit
    //Create factory
    containerfactory::SBDContainer sbdContainer;
    //Create pointer
    shared_ptr<Container> container(new Container());
    *container = sbdContainer.genSBDContainer(95.2);
    //Set Broadcaster pointer
    packetBroadcaster->setSensorBoardDataContainer(container);

    //serial_obj.disconnect();

    pbthread.join();

    return 0;
}