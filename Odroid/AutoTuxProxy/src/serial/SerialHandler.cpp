#include "serial/SerialHandler.h"
#include <containerfactory/SBDContainer.h>
#include <opendavinci/odcore/base/LIFOQueue.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <iostream>

using namespace odcore::data;
using namespace usb_connector;
using namespace containerfactory;
using namespace automotive;
using namespace std;

//void termination_handler(int);

serial::SerialHandler::SerialHandler(int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "AutoTuxProxy"),
    //usbConnector{(shared_ptr<USBConnector>)new USBConnector()},
    bufferWrapper{(shared_ptr<BufferWrapper>)new BufferWrapper()},
    interrupted{false} { }


serial::SerialHandler::~SerialHandler() {
    usbConnector->~USBConnector();
    bufferWrapper->~BufferWrapper();
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
    //usbConnector->connect();
    //usbConnector->set_buffer_wrapper(bufferWrapper);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
           odcore::data::dmcp::ModuleStateMessage::RUNNING && !interrupted) {

        // ========= READ =================================
        // call usb connector to read
        //usbConnector->read();
        // call buffer wrapper to get vector
        vector<unsigned char> v = bufferWrapper->readReceiveBuffer();
        // create a shared pointer to container
        // set the pointer in the sending thing

        if(v.size() != 0) {
            getConference().send(*SBDContainer::instance()->genSBDContainer(v));
        }

        Container c = getKeyValueDataStore().get(VehicleControl::ID());
        VehicleControl vehicleControl = c.getData<VehicleControl>();
        cout << "Speed: " << vehicleControl.getSpeed() << endl;
        cout << "Angle: " << vehicleControl.getSteeringWheelAngle() << endl;

        // ========= WRITE ================================
        // call usb connector to write the data
        //usbConnector->write();
    }

    cout << "Done with the PacketBroadCaster body" << endl;
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
