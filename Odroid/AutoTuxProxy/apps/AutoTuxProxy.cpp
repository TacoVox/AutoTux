//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <thread>
#include <opendavinci/odcore/data/Container.h>
#include "packetio/PacketBroadcaster.h"
#include "packetio/PacketReceiver.h"
#include "containerfactory/SBDContainer.h"
#include "serial/USBConnector.h"

using namespace std;
using namespace odcore::data;
using namespace packetio;
using namespace containerfactory;

int main(int argc, char **argv) {
    cout << "Testing Packet Broadcaster!" << endl;

    //Setup the broadcaster
    shared_ptr<PacketBroadcaster>
            packetBroadcaster(new PacketBroadcaster(argc, argv));
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    //Setup the receiver
    shared_ptr<PacketReceiver>
            packetReceiver(new PacketReceiver(argc, argv));
    thread prthread(&PacketReceiver::runModule, packetReceiver);

    cout << "Testing USBConnector!" << endl;
    //usb_connector::USBConnector serial_obj;
    //serial_obj.connect();
    //serial_obj.read();

    //Just for testing
    vector<unsigned char> p {0, 3, 5, 7, 7};
    packetBroadcaster->setSensorBoardDataContainer(
                SBDContainer::instance()->genSBDContainer(p));
    vector<unsigned char> p2 {0, 3, 5, 4, 5};
    packetBroadcaster->setSensorBoardDataContainer(
                SBDContainer::instance()->genSBDContainer(p2));

    //serial_obj.disconnect();

    //Wait for the threads to terminate
    pbthread.join();
    prthread.join();

    return 0;
}
