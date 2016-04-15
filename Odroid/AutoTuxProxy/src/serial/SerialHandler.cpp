#include "serial/SerialHandler.h"
#include <containerfactory/SBDContainer.h>
#include <opendavinci/odcore/base/LIFOQueue.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <iostream>
#include <cmath>

using namespace odcore::data;
using namespace usb_connector;
using namespace containerfactory;
using namespace automotive;
using namespace std;

//void termination_handler(int);

serial::SerialHandler::SerialHandler(int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "AutoTuxProxy"),
    usbConnector{(shared_ptr<USBConnector>)new USBConnector()},
    bufferWrapper{(shared_ptr<BufferWrapper>)new BufferWrapper()},
    interrupted{false} { }


serial::SerialHandler::~SerialHandler() {
    cout << "destroying serial handler object... ";
    cout << "[OK]" << endl;
}

void serial::SerialHandler::interrupt(void) {
    interrupted = true;
}

void serial::SerialHandler::setUp() {
    cout << "Started the AutoTuxProxy component..." << endl;
}

void serial::SerialHandler::tearDown() {
    cout << "Stopped the AutoTuxProxy component..." << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode
        serial::SerialHandler::body() {

    odcore::base::LIFOQueue lifoQueue;
    addDataStoreFor(lifoQueue);

    cout << "Testing USBConnector!" << endl;
    usbConnector->connect();
    usbConnector->set_buffer_wrapper(bufferWrapper);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
           odcore::data::dmcp::ModuleStateMessage::RUNNING && !interrupted) {

        // ========= READ =================================
        // call usb connector to read
        usbConnector->read();
        // call buffer wrapper to get vector
        vector<unsigned char> v = bufferWrapper->readReceiveBuffer();
        // create a shared pointer to container
        // set the pointer in the sending thing

        //If there is something to send --> send it
        if(v.size() != 0) {
            getConference().send(*SBDContainer::instance()->genSBDContainer(v));
        }

        bufferWrapper->appendSendBuffer(cdContToVec(getKeyValueDataStore().get(VehicleControl::ID())));
        cout << "Appended received data to send buffer" << endl;

        // ========= WRITE ================================
        // call usb connector to write the data
        usbConnector->write();
    }

    cout << "Done with the PacketBroadCaster body" << endl;
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

std::vector<unsigned char> serial::SerialHandler::cdContToVec(
        odcore::data::Container container) {

    VehicleControl vehicleControl = container.getData<VehicleControl>();

    unsigned char speed = (unsigned char)round(vehicleControl.getSpeed());
    unsigned char angle = (unsigned char)round(vehicleControl.getSteeringWheelAngle());
    if(angle == 0)
        angle = 90;
    unsigned char chsum = checksum({speed, angle});

    cout << "Speed: " << speed << " Angle: " << angle << endl;
    
    return {'3', ':', speed, angle, chsum, ','};
}

unsigned char serial::SerialHandler::checksum(std::vector<unsigned char> v) {
    unsigned char cs = 0;

    for(auto it = v.begin(); it < v.end(); it++)
        cs ^= *it;

   return cs;
}
