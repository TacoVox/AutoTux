#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/io/conference/ContainerConference.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>

#include <LaneFollower.h>

namespace lane {
    namespace follower {
        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::data::image;

        using namespace lane::follower;

        LaneFollower::LaneFollower(const int32_t &argc, char **argv) :
                TimeTriggeredConferenceClientModule(argc, argv, "LaneFollower"),
                m_hasAttachedToSharedImageMemory(false),
                m_sharedImageMemory(),
                m_image(NULL),
                m_debug(false),
                m_font(),
                m_previousTime(),
                m_eSum(0),
                m_eOld(0),
                m_vehicleControl() { }

        LaneFollower::~LaneFollower() { }

        void LaneFollower::setUp() {
            if (m_debug) {
                cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("WindowShowImage", 300, 100);
            }
        }

        void LaneFollower::tearDown() {
            if (m_image != NULL) {
                cvReleaseImage(&m_image);
            }
            if (m_debug) {
                cvDestroyWindow("WindowShowImage");
            }
        }


        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanefollower.debug") == 1;

            // ?
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

                if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                    //has_next_frame = readSharedImage(c);
                }

                if (has_next_frame) {
                    //processImage();
                }
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    } // follower
} // lane