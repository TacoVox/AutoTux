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

namespace lanefollower {
    using namespace std;
    using namespace odcore::base;
    using namespace odcore::data;
    using namespace odcore::data::image;

    LaneFollower::LaneFollower(const int32_t &argc, char **argv) : TimeTriggeredConferenceClientModule(argc, argv, "LaneFollower"),
        m_hasAttachedToSharedImageMemory(false),
        m_sharedImageMemory(),
        m_image(NULL),
        m_debug(false),
        m_font(),
        m_previousTime(),
        m_eSum(0),
        m_eOld(0),
        m_vehicleControl() {}

    LaneFollower::~LaneFollower() {}

    void LaneFollower::setUp() {
        if(m_debug) {
            cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
            cvMoveWindow("WindowShowImage", 300, 100);
        }
    }

    void LaneFollower::tearDown() {
        if(m_image != NULL) {
            cvReleaseImage(&m_image);
        }
        if(m_debug) {
            cvDestroyWindow("WindowShowImage");
        }
    }

    bool LaneFollower::readSharedImage(odcore::data::Container &c) {
        bool retVal = false;

        if(c.getDataType() == odcore::data::image::SharedImage::ID()) {
            SharedImage si = c.getData<SharedImage>();

            // Have we already attached to the shared memory?
            if(!m_hasAttachedToSharedImageMemory) {
                m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(si.getName());
            }

            // Could we attach succressfully?
            if(m_sharedImageMemory->isValid()) {
                Lock l(m_sharedImageMemory);
                const uint32_t numberOfChannels = 3;

                if(m_image == NULL) {
                    m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
                }

                // Copy image. EXPENSIVE!
                // TODO Check for proper solution
                if(m_image != NULL) {
                    memcpy(m_image->imageData, m_sharedImageMemory->getSharedMemory(), si.getWidth() * si.getHeight() * numberOfChannels);
                }

                // Mirror image? Reason?
                cvFlip(m_image, 0, -1);

                retVal = true;
            }
        }
        return retVal;
    }

    void LaneFollower::processImage() {
        static bool useRightLaneMarking = true;
        double e = 0;

        const int32_t CONTROL_SCANLINE = 462;
        const int32_t distance = 280;

        TimeStamp beforeProcessing; // Keep before processing loop

        for(int32_t y = m_image->height - 8; y > m_image->height * .6; y -= 10) {
            CvScalar pixelLeft;
            CvPoint left;
            left.y = y;
            left.x = -1;
            for(int x = m_image->width/2; x > 0; x--) {
                pixelLeft = cvGet2D(m_image, y, x);
                if(pixelLeft.val[0] >= 200) {
                    left.x = x;
                    break;
                }
            }

            CvScalar pixelRight;
            CvPoint right;
            right.y = y;
            right.x = -1;
            for(int x = m_image->width/2; x < m_image->width; x++) {
                pixelRight = cvGet2D(m_image, y, x);
                if(pixelRight.val[0] >= 200) {
                    right.x = x;
                    break;
                }
            }
        }

        TimeStamp afterProcessing; // Keep after processing loop
        cerr << "Processing time: " << (afterProcessing.toMicroseconds() - beforeProcessing.toMicroseconds())/1000.0 << "ms." << endl;

        // Show image
        if(m_debug) {
            if(m_image != NULL) {
                cvShowImage("WindowShowImage", m_image);
                cvWaitKey(10);
            }
        }
    }

    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
        // Get configuration
        KeyValueConfiguration kv = getKeyValueConfiguration();
        m_debug = kv.getValue<int32_t>("lanefollower.debug") == 1;

        // ?
        while(getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
            bool has_next_frame = false;

            Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

            if(c.getDataType() == odcore::data::image::SharedImage::ID()) {
                has_next_frame = readSharedImage(c);
            }

            if(true == has_next_frame) {
                processImage();
            }
        }

        return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
    }
} // lanefollower