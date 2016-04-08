#include <iostream>
#include <memory>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>

#include "LaneFollower.h"

// Used for debugging
#include <opendavinci/odtools/player/Player.h>

namespace lane {
    namespace follower {
        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::data::image;
        using namespace odcore::wrapper;

        using namespace lane::follower;

        // Debugging use only
        using namespace odtools::player;

        LaneFollower::LaneFollower(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "LaneDetector"),
            m_hasAttachedToSharedImageMemory(false),
            m_sharedImageMemory(),
            m_image(NULL),
            m_previousTime(),
            m_font(),
            m_debug(false),
            m_vehicleControl() {}

        LaneFollower::~LaneFollower() {}

        // This method will run before body()
        void LaneFollower::setUp() {
            // Set up debug window
            if(m_debug) {
                cvNamedWindow("Debug Window", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("Debug Window", 300, 100);
            }

            // Set up shared memory for storing things
            try {
                shared_ptr<SharedMemory> sharedMemory(SharedMemoryFactory::createSharedMemory("TEST", 64));
            } catch(string &ex) {
                cerr << "Shared memory could not be created: " << ex << endl;
            }
        }

        // This method will run after return from body()
        void LaneFollower::tearDown() {
            if(m_image != NULL) {
                cvReleaseImage(&m_image);
            }

            if(m_debug) {
                cvvDestroyWindow("Debug Window");
            }
        }

        bool LaneFollower::readSharedImage(odcore::data::Container &c) {
            bool returnValue = false;

            if(c.getDataType() == odcore::data::image::SharedImage::ID()) {
                SharedImage si = c.getData<SharedImage>();

                // Have we already attached to the shared memory containing the image?
                if(!m_hasAttachedToSharedImageMemory) {
                    m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(si.getName());
                }

                // Did we successfully connect?
                if(m_sharedImageMemory->isValid()) {
                    Lock l(m_sharedImageMemory);

                    if(m_image == NULL) {
                        m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, si.getBytesPerPixel());
                    }

                    // Copy the image to current process space
                    if(m_image != NULL) {
                        memcpy(m_image->imageData, m_sharedImageMemory->getSharedMemory(), si.getWidth() * si.getHeight() * si.getBytesPerPixel());
                    }

                    // Mirror image?
                    // NOTE: Only for simulator
                    cvFlip(m_image, 0, -1);

                    returnValue = true;
                }
            }
            return returnValue;
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            double e = 0; //?
            const int32_t CONTROL_SCANLINE = 462; //?
            const int32_t distance = 280; //?

            TimeStamp beforeProcessing;

            for(int32_t y = m_image->height - 8; y > m_image->height * .5; y -= 10) {
                CvScalar pixelLeft, pixelRight;
                CvPoint left, right;

                left.y = y;
                left.x = -1;
                for (int x = m_image->width / 2; x > 0; x--) {
                    pixelLeft = cvGet2D(m_image, y, x);
                    if (pixelLeft.val[0] >= 200) {
                        left.x = x;
                        break;
                    }
                }

                right.y = y;
                right.x = -1;
                for (int x = m_image->width / 2; x < m_image->width; x++) {
                    pixelRight = cvGet2D(m_image, y, x);
                    if (pixelRight.val[0] >= 200) {
                        right.x = x;
                        break;
                    }
                }

                // Attach and store in the shared memory.
                if (y == CONTROL_SCANLINE) {
                    shared_ptr<SharedMemory> sharedMemory(SharedMemoryFactory::attachToSharedMemory("TEST"));
                    if (sharedMemory->isValid()) {
                        Lock l(sharedMemory);

                    }
                    this->m_distToLeftMarking = m_image->width/2 - left.x;
                    this->m_distToRightMarking = right.x - m_image->width/2;
                }

                /**
                 * TODO
                 * Decide on a proper scanline to use when calculating position.
                 * Defined as CONTROL_SCANLINE further up.
                 * Take into account the deviation error. Examples in the automotive/miniature.
                 *
                 * Research into the "Twiddle" algorithm.
                 *
                 * Later store/send position to the decision maker.
                 */

                if (m_debug) {
                    if (left.x > 0) {
                        cvLine(m_image, cvPoint(m_image->width / 2, y), left, CV_RGB(0, 255, 0), 1, 8);
                        stringstream sstr;
                        sstr << (m_image->width / 2 - left.x);
                        cvPutText(m_image, sstr.str().c_str(), cvPoint(m_image->width / 2 - 100, y - 2), &m_font,
                                  CV_RGB(0, 255, 0));
                    }
                    if (right.x > 0) {
                        cvLine(m_image, cvPoint(m_image->width / 2, y), right, CV_RGB(255, 0, 0), 1, 8);
                        stringstream sstr;
                        sstr << (right.x - m_image->width / 2);
                        cvPutText(m_image, sstr.str().c_str(), cvPoint(m_image->width / 2 + 100, y - 2), &m_font,
                                  CV_RGB(255, 0, 0));
                    }
                }
            }

            // Debugging; show image. Keep at end.
            if(m_debug) {
                if(m_image != NULL) {
                    cvShowImage("Camera Image", m_image);
                    cvWaitKey(10);
                }
            }

            double desiredSteering = m_vehicleControl.getSteeringWheelAngle();

            //cerr << "Dist to left marking: " << m_distToLeftMarking << " : ";
            //cerr << "Dist to right marking: " << m_distToRightMarking << endl;

            if(m_distToLeftMarking > 240 && m_distToLeftMarking < 320 &&
                    m_distToRightMarking > 240 && m_distToRightMarking < 320) {
                desiredSteering = 0;
                cerr << "Going straight!" << endl;
            }
            else if((m_distToRightMarking > 320 || m_distToRightMarking < 0) &&
                        m_distToLeftMarking < 270 && m_distToLeftMarking > 0) {
                desiredSteering += 2;
                cerr << "Turning right! Left: " << m_distToLeftMarking << " & Right: " << m_distToRightMarking << endl;
            }
            else if((m_distToLeftMarking > 320 || m_distToLeftMarking < 0) &&
                        m_distToRightMarking < 230 && m_distToRightMarking > 0) {
                desiredSteering -= 2;
                cerr << "Turning left! Left: " << m_distToLeftMarking << " & Right: " << m_distToRightMarking << endl;
            }

            // Panic (turn right)
            else if(m_distToLeftMarking < 100) {
                desiredSteering += 2;
                cerr << "Panic! (right)" << endl;
            }

            // Panic (turn left)
            else if(m_distToLeftMarking < 100)
            {
                desiredSteering -= 2;
                cerr << "Panic! (left)" << endl;
            }
            else {
                desiredSteering = 0;
                cerr << "Nothing." << endl;
            }

            TimeStamp afterImageProcessing;
            //clog << "Processing time: " << (afterImageProcessing.toMicroseconds() - beforeProcessing.toMicroseconds()) / 1000.0 << "ms." << endl;

            //cerr << "Angle: " << desiredSteering << endl;

            m_vehicleControl.setSpeed(5);
            m_vehicleControl.setSteeringWheelAngle(desiredSteering);
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanedetector.debug") == 1;

            // Initialize OpenCV fonts
            const double hscale = 0.4;
            const double vscale = 0.3;

            cvInitFont(&m_font, CV_FONT_NORMAL, hscale, vscale);

            // ?
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

                if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                    has_next_frame = readSharedImage(c);
                }

                if (has_next_frame) {
                    processImage();
                }

                Container c2(m_vehicleControl);

                getConference().send(c2);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    } // detector
} // lane