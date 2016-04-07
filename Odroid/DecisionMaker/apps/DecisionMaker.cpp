//
// Created by Jonas Kahler on 4/7/16.
//
#include "packetio/PacketBroadcaster.h"
#include "overtaker/Overtaker.h"
#include "parker/Parker.h"
#include <opendavinci/odcore/data/Container.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace packetio;
using namespace odcore::data;
using namespace automotive;
//using namespace overtaker;
//using namespace parker;

void startDecisionMaker(int, char**);

int main(int argc, char **argv) {
    cout << "Started the AutoTux DecisionMaker..." << endl;
    shared_ptr<PacketBroadcaster> packetBroadcaster(new PacketBroadcaster(argc, argv));
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);

    // Initialize Overtaker and Parker Threads
    startDecisionMaker(argc, argv);

    VehicleControl vehicleControl;
    vehicleControl.setSpeed(10.0);
    vehicleControl.setSteeringWheelAngle(90.0);

    Container container(vehicleControl);

    shared_ptr<Container> containerptr(new Container(vehicleControl));

    packetBroadcaster->setControlDataContainer(containerptr);

    pbthread.join();
    return 0;
}

void startDecisionMaker(int argc, char **argv) {

    //thread overtaker (initializeOvertaker, argc, argv);
    //thread parker (initializeParker, argc, argv);
}

void initializeOvertaker(int argc, char **argv){
    //overtaker::Overtaker ovt (argc, argv);
    //ovt.runModule();
}

void initializeParker(int argc, char **argv){
    //parker::Parker prk (argc, argv);
    //prk.runModule();
}

