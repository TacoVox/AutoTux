#include "serial/SerialHandler.h"
#include "containerfactory/SBDContainer.h"
#include <thread>
#include <iostream>

using namespace odcore::data;
using namespace packetio;
using namespace usb_connector;
using namespace containerfactory;
using namespace std;

void termination_handler(int);

serial::SerialHandler::SerialHandler(int32_t &argc, char **argv) :
    packetBroadcaster{(shared_ptr<PacketBroadcaster>)new PacketBroadcaster(argc, argv)},
    packetReceiver{(shared_ptr<PacketReceiver>)new PacketReceiver(argc, argv)},
    usbConnector{(shared_ptr<USBConnector>)new USBConnector()},
    bufferWrapper{(shared_ptr<BufferWrapper>)new BufferWrapper()},
    interrupted{false}
{
    //Set the bufferWrapperPointer in the packetReceiver
    packetReceiver->setBufferWrapper(bufferWrapper);
}


serial::SerialHandler::~SerialHandler()
{
    packetBroadcaster->~PacketBroadcaster();
    packetReceiver->~PacketReceiver();
    usbConnector->~USBConnector();
    bufferWrapper->~BufferWrapper();
}


void serial::SerialHandler::run(void)
{
    thread pbthread(&PacketBroadcaster::runModule, packetBroadcaster);
    pbthread.detach();
    thread prthread(&PacketReceiver::runModule, packetReceiver);
    prthread.detach();

    cout << "Testing USBConnector!" << endl;
    usbConnector->connect();
    usbConnector->set_buffer_wrapper(bufferWrapper);

    // do the main loop for reading and writing here
    // while we are connected
    while (!interrupted) {
        // ========= READ =================================
        // call usb connector to read
        usbConnector->read();
        // call buffer wrapper to get vector
        vector<unsigned char> v = bufferWrapper->readReceiveBuffer();
        // create a shared pointer to container
        // set the pointer in the sending thing
        packetBroadcaster->setSensorBoardDataContainer(
                SBDContainer::instance()->genSBDContainer(v));
        // ========= WRITE ================================
        // call usb connector to write the data
        usbConnector->write();

        // sleep for 1 sec, just for output
        std::this_thread::sleep_for(chrono::milliseconds(500));
    }

    usbConnector->disconnect();
}


void serial::SerialHandler::interrupt(void)
{
    interrupted = true;
}
