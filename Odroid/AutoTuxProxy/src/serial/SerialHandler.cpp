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
    bufferWrapper = (shared_ptr<BufferWrapper>)new BufferWrapper();
}

void serial::SerialHandler::run(void)
{
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    thread prthread(&PacketReceiver::runModule, packetReceiver);

    cout << "Testing USBConnector!" << endl;
    usbConnector->connect();
    usbConnector->read();

    // do the main loop for reading and writing here
    // while we are connected
    // while (1) {
    //      ========= READ =================================
    //      call usb connector to read
    //      call buffer wrapper to get vector
    //      create a shared pointer to container
    //      set the pointer in the sending thing (see below)
    //
    //      ========= WRITE ================================
    //      call buffer wrapper to get data to write
    //      call usb connector to write the data
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
