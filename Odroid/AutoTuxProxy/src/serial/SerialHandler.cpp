#include "serial/SerialHandler.h"
#include "containerfactory/SBDContainer.h"
#include "serial/USBConnector.h"
#include <thread>
#include <iostream>

using namespace odcore::data;
using namespace packetio;
using namespace containerfactory;
using namespace std;

serial::SerialHandler::SerialHandler(int32_t &argc, char **argv)
{
    //Setup the broadcaster
    packetBroadcaster = (shared_ptr<PacketBroadcaster>)new PacketBroadcaster(argc, argv);
    packetReceiver = (shared_ptr<PacketReceiver>)new PacketReceiver(argc, argv);
}

void serial::SerialHandler::run(void) {
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    thread prthread(&PacketReceiver::runModule, packetReceiver);

    cout << "Testing USBConnector!" << endl;
    usb_connector::USBConnector serial_obj;
    serial_obj.connect();
    serial_obj.read();

    //Just for testing
    vector<unsigned char> p {0, 3, 5, 7, 7};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p));
    vector<unsigned char> p2 {0, 3, 5, 4, 5};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p2));

    serial_obj.disconnect();

    //Wait for the threads to terminate
    pbthread.join();
    prthread.join();
}