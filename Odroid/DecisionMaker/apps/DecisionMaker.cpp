//
// Created by Jonas Kahler on 4/7/16.
//
#include "packetio/PacketBroadcaster.h"
#include <opendavinci/odcore/data/Container.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace packetio;
using namespace odcore::data;
using namespace automotive;

int main(int argc, char **argv) {
    cout << "Started the AutoTux DecisionMaker..." << endl;
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(argc, argv));
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);

    shared_ptr<Container> containerptr;

    packetBroadcaster->setControlDataContainer(containerptr);

    VehicleControl vehicleControl;
    vehicleControl.setSpeed(10.0);
    vehicleControl.setSteeringWheelAngle(90.0);

    Container container(vehicleControl);

    *containerptr = container;

    pbthread.join();
    return 0;
}
