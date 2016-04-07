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

    cout << "Testing USBConnector!" << endl;
    usb_connector::USBConnector serial_obj;
    serial_obj.connect();
    serial_obj.read();

    serial_obj.disconnect();

    pbthread.join();

    return 0;
}
