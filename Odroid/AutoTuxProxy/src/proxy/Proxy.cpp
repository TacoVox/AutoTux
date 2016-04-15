#include "proxy/Proxy.h"
#include <containerfactory/SBDContainer.h>
#include <opendavinci/odcore/base/LIFOQueue.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <iostream>
#include <cmath>

using namespace odcore::data;
using namespace containerfactory;
using namespace automotive;
using namespace std;


proxy::Proxy::Proxy(int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "AutoTuxProxy") { }

proxy::Proxy::Proxy(int32_t &argc, char **argv,
                    std::shared_ptr<serial::BufferWrapper> bw) :
        TimeTriggeredConferenceClientModule(argc, argv, "AutoTuxProxy"),
    bufferWrapper{bw},
    interrupted{false} {}


proxy::Proxy::~Proxy() {
    cout << "destroying serial handler object... ";
    cout << "[OK]" << endl;
}

void proxy::Proxy::setUp() {
    cout << "Started the AutoTuxProxy component..." << endl;
}

void proxy::Proxy::tearDown() {
    cout << "Stopped the AutoTuxProxy component..." << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode
    proxy::Proxy::body() {

    odcore::base::LIFOQueue lifoQueue;
    addDataStoreFor(lifoQueue);

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
           odcore::data::dmcp::ModuleStateMessage::RUNNING && !interrupted) {

        // ========= READ =================================
        // call buffer wrapper to get vector
        vector<unsigned char> v = bufferWrapper->readReceiveBuffer();

        //If there is something to send --> send it
        if(v.size() != 0) {
            getConference().send(*SBDContainer::instance()->genSBDContainer(v));
        }

        bufferWrapper->appendSendBuffer(cdContToVec(getKeyValueDataStore().get(VehicleControl::ID())));
        cout << "Appended received data to send buffer" << endl;
    }

    cout << "Done with the PacketBroadCaster body" << endl;
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

std::vector<unsigned char> proxy::Proxy::cdContToVec(
        odcore::data::Container container) {

    VehicleControl vehicleControl = container.getData<VehicleControl>();

    unsigned char speed = (unsigned char)round(vehicleControl.getSpeed());
    unsigned char angle = (unsigned char)round(vehicleControl.getSteeringWheelAngle());

    //Check if angle not from empty packet (normally this should only occur
    //before the decision maker sent out any values
    if(angle == 0)
        angle = 90;

    //Generate the checksum for the control values
    unsigned char chsum = checksum({speed, angle});

    //Some fancy debug output
    cout << "Speed: " << speed << " Angle: " << angle << endl;

    return {'3', ':', speed, angle, chsum, ','};
}

unsigned char proxy::Proxy::checksum(std::vector<unsigned char> v) {
    unsigned char cs = 0;

    for(auto it = v.begin(); it < v.end(); it++)
        cs ^= *it;

   return cs;
}

void proxy::Proxy::interrupt(void) {
    interrupted = true;
}

void proxy::Proxy::setBufferWrapper(std::shared_ptr<serial::BufferWrapper> bw) {
    this->bufferWrapper = bw;
}
