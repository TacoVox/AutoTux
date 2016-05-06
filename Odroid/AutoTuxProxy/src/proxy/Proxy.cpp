#include <cmath>
#include <ctype.h>
#include <iostream>
#include <containerfactory/SBDContainer.h>
#include <containerfactory/VDContainer.h>
#include <opendavinci/odcore/base/LIFOQueue.h>
#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <automotivedata/generated/automotive/vehicle/LightSystem.h>
#include "proxy/Proxy.h"
#include "camera/OpenCVCamera.h"

namespace proxy {
    using namespace std;
    using namespace odcore::base;
    using namespace odcore::data;
    using namespace odtools::recorder;

    using namespace containerfactory;
    using namespace automotive;
    using namespace automotive::vehicle;
    using namespace proxy::camera;

    Proxy::Proxy(int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "Proxy"),
            interrupted{false},
            bufferWrapper{},
            m_recorder{},
            m_camera{}
    { }

    Proxy::Proxy(int32_t &argc, char **argv, shared_ptr<serial::SerialBuffer> bw) :
            TimeTriggeredConferenceClientModule(argc, argv, "Proxy"),
            interrupted{false},
            bufferWrapper{bw},
            m_recorder{},
            m_camera{}
    { }


    Proxy::~Proxy() {
        cout << "destroying serial handler object... ";
        cout << "[OK]" << endl;
    }

    void Proxy::setUp() {
        cout << "Started the AutoTuxProxy component..." << endl;

        KeyValueConfiguration kv = getKeyValueConfiguration();

        // Recording setup
        const bool useRecorder = kv.getValue<uint32_t>("proxy.camera.useRecorder") == 1;
        if(useRecorder) {
            stringstream recordingURL;
            recordingURL << "file://" << "proxy_camera_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";

            const uint32_t MEMORY_SEGMENT_SIZE = kv.getValue<uint32_t>("global.buffer.memorySegmentSize");
            const uint32_t NUMBER_OF_SEGMENTS = kv.getValue<uint32_t>("global.buffer.numberOfMemorySegments");
            const bool DUMP_SHARED_DATA = kv.getValue<uint32_t>("proxy.camera.dumpSharedData") == 1;
            const bool THREADING = true;

            m_recorder = unique_ptr<Recorder>(new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA));
        }

        // Create camera grabber
        const string NAME = kv.getValue<string>("proxy.camera.name");
        string TYPE = kv.getValue<string>("proxy.camera.type");
        transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
        const uint32_t ID = kv.getValue<uint32_t>("proxy.camera.id");
        const uint32_t WIDTH = kv.getValue<uint32_t>("proxy.camera.width");
        const uint32_t HEIGHT = kv.getValue<uint32_t>("proxy.camera.height");
        const uint32_t BPP = kv.getValue<uint32_t>("proxy.camera.bpp");

        if(TYPE.compare("opencv") == 0) {
            // This is red. But there is no error.
            m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
        } else {
            cerr << "Wrong camera type, use 'OpenCV'" << endl;
        }

        if(m_camera.get() == NULL) {
            cerr << "No valid camera." << endl;
        }
    }

    void Proxy::tearDown() {
        cout << "Stopped the AutoTuxProxy component..." << endl;
    }

    void Proxy::distribute(Container c) {
        if(m_recorder.get() != NULL) {
            c.setReceivedTimeStamp(TimeStamp());
            m_recorder->store(c);
        }
        getConference().send(c);
    }

    dmcp::ModuleExitCodeMessage::ModuleExitCode Proxy::body() {

        LIFOQueue lifoQueue;
        addDataStoreFor(lifoQueue);

        while (getModuleStateAndWaitForRemainingTimeInTimeslice() == dmcp::ModuleStateMessage::RUNNING && !interrupted) {
            // ========= READ =================================
            // call buffer wrapper to get vector
            vector<unsigned char> v = bufferWrapper->readReceiveBuffer();

            //If there is something to send --> send it
            if (v.size() == 11) {
                getConference().send(*SBDContainer::instance()->
                        genSBDContainer(v));
                getConference().send(*VDContainer::instance()->
                        genVDContainer(v));
            }

            Container vcc = getKeyValueDataStore().get(VehicleControl::ID());
            Container lsc = getKeyValueDataStore().get(LightSystem::ID());
            bufferWrapper->appendSendBuffer(contToVec(vcc, lsc));

            // Camera things
            if(m_camera.get() != NULL) {
                image::SharedImage si = m_camera->capture();
                Container c(si); // This is also red, but it works.
                distribute(c);
            }
        }

        return dmcp::ModuleExitCodeMessage::OKAY;
    }

    vector<unsigned char> Proxy::contToVec(Container vcc, Container lsc) {

        VehicleControl vehicleControl = vcc.getData<VehicleControl>();
        LightSystem lightSystem = lsc.getData<LightSystem>();

        //Angle conversion
        int degrees = (int)(vehicleControl.getSteeringWheelAngle() * 180 / MATH_PI);
        unsigned char angle = (unsigned char)(degrees + 90);

        // Speed conversion
        double delta = 0.0001;
        unsigned char speed = 1; // Assume stopped
        if (vehicleControl.getSpeed() > 1 + delta) {
            speed = 3;
        } else if (vehicleControl.getSpeed() > 0 + delta) {
            speed = 2; // Forward
        } else if (vehicleControl.getSpeed() < 0 - delta) {
            speed = 0; // backward
        }

        //For debbug set the flashing light right
        //vehicleControl.setFlashingLightsLeft(true);
        vehicleControl.setFlashingLightsRight(true);

        unsigned char lights = (unsigned char)vehicleControl.getBrakeLights();
        lights = lights | (unsigned char)(lightSystem.getReverseLight() << 1);
        lights = lights | (unsigned char)(vehicleControl.getFlashingLightsLeft() << 2);
        lights = lights | (unsigned char)(vehicleControl.getFlashingLightsRight() << 3);

        //Generate the checksum for the control values
        unsigned char chsum = checksum({speed, angle, lights});

        //Some fancy debug output
        //cout << "Speed: " << speed << " Angle: " << angle << endl;

        if(Proxy::getVerbosity() == 2) {
            vector<unsigned char> debugvec = {'4', ':', speed, angle, lights, chsum, ','};
            for(auto it = debugvec.begin(); it < debugvec.end(); it++) {
                printf("%u", *it);
            }
            cout << endl;
        }
        return {'4', ':', speed, angle, lights, chsum, ','};
    }

    unsigned char Proxy::checksum(vector<unsigned char> v) {
        unsigned char cs = 0;

        for (auto it = v.begin(); it < v.end(); it++)
            cs = (unsigned char)(cs ^ *it);

        return cs;
    }

    void Proxy::interrupt(void) {
        interrupted = true;
    }

    void Proxy::setBufferWrapper(shared_ptr<serial::SerialBuffer> bw) {
        this->bufferWrapper = bw;
    }
}
