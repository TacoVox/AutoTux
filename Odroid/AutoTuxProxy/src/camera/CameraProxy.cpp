//
// Created by max on 4/5/16.
//

#include <ctype.h>
#include <cstring>
#include <iostream>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>

#include "camera/OpenCVCamera.h"

#include "camera/CameraProxy.h"

namespace proxy {
    namespace camera {
        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odtools::recorder;

        CameraProxy::CameraProxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "CameraProxy"),
            m_recorder(),
            m_camera()
        {}

        CameraProxy::~CameraProxy() {}

        void CameraProxy::setUp() {
            if(getFrequency() < 20) {
                cerr << endl << "CameraProxy: Warning! Running with low frequency" << endl << endl;
            }

            KeyValueConfiguration kv = getKeyValueConfiguration();

            //TODO Fix configuration file. Can only get values using the filename?

            const bool useRecorder = kv.getValue<uint32_t>("cameraproxy.useRecorder") == 1;
            if(useRecorder) {
                stringstream recordingURL;
                recordingURL << "file://" << "proxy_camera_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";

                const uint32_t MEMORY_SEGMENT_SIZE = kv.getValue<uint32_t>("global.buffer.memorySegmentSize");
                const uint32_t NUMBER_OF_SEGMENTS = kv.getValue<uint32_t>("global.buffer.numberOfMemorySegments");
                const bool THREADING = true;
                const bool DUMP_SHARED_DATA = kv.getValue<uint32_t>("cameraproxy.dumpshareddata") == 1;

                m_recorder = unique_ptr<Recorder>(new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA));
            }

            // Create camrea grabber.
            const string NAME = getKeyValueConfiguration().getValue<string>("cameraproxy.name");
            string TYPE = kv.getValue<string>("cameraproxy.type");
            transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
            const uint32_t ID = kv.getValue<uint32_t>("cameraproxy.id");
            const uint32_t WIDTH = kv.getValue<uint32_t>("cameraproxy.width");
            const uint32_t HEIGHT = kv.getValue<uint32_t>("cameraproxy.height");
            const uint32_t BPP = kv.getValue<uint32_t>("cameraproxy.bpp");

            if(TYPE.compare("opencv") == 0) {
                // TODO Error? No compiling errors.
                m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
            } else {
                cerr << "Wrong camera type, use 'OpenCV'" << endl;
            }

            if(m_camera.get() == NULL) {
                cerr << "NO VALID CAMERA GTFO." << endl;
            }
        }

        void CameraProxy::tearDown() {

        }

        void CameraProxy::distribute(Container c) {
            if(m_recorder.get() != NULL) {
                c.setReceivedTimeStamp(TimeStamp());
                m_recorder->store(c);
            }

            getConference().send(c);
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode CameraProxy::body() {
            uint32_t captureCounter = 0;
            while(getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                if(m_camera.get() != NULL) {
                    odcore::data::image::SharedImage si = m_camera->capture();

                    // WHY IS THIS GIVING AN ERROR?
                    Container c(si);

                    distribute(c);
                    captureCounter++;
                }
                // Get sensors? Other module?
            }

            cout << "CameraProxy: Captured " << captureCounter << " frames." << endl;

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    }
}
