//
// Created by Jonas Kahler on 4/7/16.
//
#include "packetio/PacketBroadcaster.h"
#include <opendavinci/odcore/data/Container.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace packetio;
using namespace odcore::data;

int main(int argc, char **argv) {
    cout << "Started the AutoTux DecisionMaker..." << endl;
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(argc, argv));
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);

    shared_ptr<Container> container(new Container());

    packetBroadcaster->setControlDataContainer(container);

    pbthread.join();
    return 0;
}
