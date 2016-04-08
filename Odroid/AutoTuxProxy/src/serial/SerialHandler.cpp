#include "serial/SerialHandler.h"
#include "containerfactory/SBDContainer.h"
#include <thread>
#include <iostream>

using namespace odcore::data;
using namespace packetio;
using namespace usb_connector;
using namespace containerfactory;
using namespace std;

serial::SerialHandler::SerialHandler(int32_t &argc, char **argv)
{
    //Setup the broadcaster
    packetBroadcaster = (shared_ptr<PacketBroadcaster>)new PacketBroadcaster(argc, argv);
    packetReceiver = (shared_ptr<PacketReceiver>)new PacketReceiver(argc, argv);
    usbConnector = (shared_ptr<USBConnector>)new USBConnector();
}

void serial::SerialHandler::run(void)
{
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    thread prthread(&PacketReceiver::runModule, packetReceiver);

    cout << "Testing USBConnector!" << endl;
    usbConnector->connect();
    usbConnector->read();

    // do the main loop for reading and writing here
    // while (1) {
    //      do some stuff
    // }

    //Just for testing
    vector<unsigned char> p {0, 3, 5, 7, 7};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p));
    vector<unsigned char> p2 {0, 3, 5, 4, 5};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p2));

    usbConnector->disconnect();
}
