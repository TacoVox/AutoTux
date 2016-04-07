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
using namespace packetio;
using namespace containerfactory;

int main(int argc, char **argv) {
    cout << "Testing Packet Broadcaster!" << endl;

    std::shared_ptr<PacketBroadcaster>
            packetBroadcaster(new PacketBroadcaster(argc, argv));
    std::thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);

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

    pbthread.join();

    return 0;
}
