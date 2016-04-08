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
    //Setup the packetIO stuff
    packetBroadcaster = (shared_ptr<PacketBroadcaster>)new PacketBroadcaster(argc, argv);
    packetReceiver = (shared_ptr<PacketReceiver>)new PacketReceiver(argc, argv);
    //Setup the usbconnector
    usbConnector = (shared_ptr<USBConnector>)new USBConnector();
    //Setup the wrapper class for the buffer
    bufferWrapper = (shared_ptr<BufferWrapper>)new BufferWrapper();

    //Set the bufferWrapperPointer in the packetReceiver
    packetReceiver->setBufferWrapper(bufferWrapper);
}

serial::SerialHandler::~SerialHandler() {
    packetBroadcaster->~PacketBroadcaster();
    packetReceiver->~PacketReceiver();
    usbConnector->~USBConnector();
    bufferWrapper->~BufferWrapper();
}

void serial::SerialHandler::run(void)
{
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    thread prthread(&PacketReceiver::runModule, packetReceiver);

    cout << "Testing USBConnector!" << endl;
    usbConnector->connect();
    usbConnector->set_buffer_wrapper(bufferWrapper);


    // do the main loop for reading and writing here
    // while we are connected
    while (1) {
    //      ========= READ =================================
    //      call usb connector to read
        usbConnector->read();
        vector<unsigned char> v = bufferWrapper->readReceiveBuffer();
        packetBroadcaster->setSensorBoardDataContainer(
                SBDContainer::instance()->genSBDContainer(v));
    //      call buffer wrapper to get vector
    //      create a shared pointer to container
    //      set the pointer in the sending thing (see below)
    //
    //      ========= WRITE ================================
    //      call buffer wrapper to get data to write
    //      call usb connector to write the data
    }

    //Just for testing
    vector<unsigned char> p {0, 3, 5, 7, 7};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p));
    vector<unsigned char> p2 {0, 3, 5, 4, 5};
    packetBroadcaster->setSensorBoardDataContainer(
            SBDContainer::instance()->genSBDContainer(p2));

    usbConnector->disconnect();
}
