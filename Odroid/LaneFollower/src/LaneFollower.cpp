#include <iostream>
#include <memory>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>

#include "LaneFollower.h"

// Used for debugging
#include <opendavinci/odtools/player/Player.h>

namespace lane {
    namespace follower {
        using namespace std;
        using namespace cv;
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
                m_image(),
                m_previousTime(),
                m_debug(false),
                m_vehicleControl(),
                laneRecommendation() {}

        LaneFollower::~LaneFollower() { }

        // This method will run before body()
        void LaneFollower::setUp() {
            // Set up debug window
            if (m_debug) {
                cvNamedWindow("Debug Window", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("Debug Window", 300, 100);
            }
        }

        // This method will run after return from body()
        void LaneFollower::tearDown() {
            if (!m_image.empty()) {
                //cvReleaseImage(&m_image);
                m_image.deallocate();
            }

            if (m_debug) {
                cvvDestroyWindow("Debug Window");
            }
        }

        bool LaneFollower::readSharedImage(odcore::data::Container &c) {
            bool returnValue = false;

            if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                SharedImage si = c.getData<SharedImage>();

                // Have we already attached to the shared memory containing the image?
                if (!m_hasAttachedToSharedImageMemory) {
                    m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(si.getName());
                }

                // Did we successfully connect?
                if (m_sharedImageMemory->isValid()) {
                    Lock l(m_sharedImageMemory);
                    // Create image(cv::Mat) if empty.
                    if (m_image.empty()) {
                        m_image.create(si.getHeight(), si.getWidth(), CV_8UC3);
                    } else {
                        // Copy image data form SharedImageMemory
                        memcpy(m_image.data, m_sharedImageMemory->getSharedMemory(),
                               si.getHeight() * si.getWidth() * si.getBytesPerPixel());
                    }

                    // Mirror image
                    // NOTE: For simulator.
                    // flip(m_image, m_image, 0);
                    returnValue = true;
                }
            }
            return returnValue;
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            const int32_t CONTROL_SCANLINE = 462;
            Point left, right;

            for (int32_t y = m_image.rows - 8; y > m_image.rows * .5; y -= 10) {

                left.y = y;
                left.x = -1;

                for (int x = m_image.cols / 2; x > 0; x--) {
                    if (m_image.at<Vec3b>(Point(x, y))[0] > 200) {
                        left.x = x;
                        break;
                    }
                }

                right.y = y;
                right.x = -1;
                for (int x = m_image.cols / 2; x < m_image.cols; x++) {
                    if (m_image.at<Vec3b>(Point(x, y))[0] > 200) {
                        right.x = x;
                        break;
                    }
                }

                if (y == CONTROL_SCANLINE) {
                    m_distToLeftMarking = m_image.cols / 2 - left.x;
                    m_distToRightMarking = right.x - m_image.cols / 2;
                }

                if (m_debug) {
                    if (left.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), left, CV_RGB(0, 255, 0));
                        stringstream sstr;
                        sstr << (m_image.cols / 2 - left.x);
                        putText(m_image, sstr.str().c_str(), Point(m_image.cols / 2 - 100, y - 2), FONT_HERSHEY_PLAIN,
                                0.5, CV_RGB(0, 255, 0));
                    }
                    if (right.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), right, CV_RGB(200, 0, 0));
                        stringstream sstr;
                        sstr << (right.x - m_image.cols / 2);
                        putText(m_image, sstr.str().c_str(), Point(m_image.cols / 2 + 100, y - 2), FONT_HERSHEY_PLAIN,
                                0.5, CV_RGB(255, 0, 0));
                    }
                }
            }

            if (m_debug) {
                if (m_image.data != NULL) {
                    imshow("Camera Image", m_image);
                    waitKey(10);
                }
            }

            laneRecommendation.setAngle(laneRecommendation.getAngle());
            laneRecommendation.setQuality(true);

            // cerr << "Dist to left marking: " << m_distToLeftMarking << " : ";
            // cerr << "Dist to right marking: " << m_distToRightMarking << endl;

            // If the car is close enough to the middle of the road, just
            // keep going forward.
            if (m_distToLeftMarking > 240 && m_distToLeftMarking < 320 &&
                m_distToRightMarking > 240 && m_distToRightMarking < 320) {
                laneRecommendation.setAngle(0);
                cerr << "Going straight!" << endl;
            }

                // Whenever the left line gets too close and the right
                // line gets too far away, make a right turn.
                // NOTE: To keep further away from the left lane, the distance
                // it waits until it turns is less than a left turn.
            else if ((m_distToRightMarking > 320 || m_distToRightMarking < 0) &&
                     m_distToLeftMarking < 270 && m_distToLeftMarking > 0) {
                laneRecommendation.setAngle(2.0);
                cerr << "Turning right! Left: " << m_distToLeftMarking << " & Right: " << m_distToRightMarking << endl;
            }

                // Whenever the right line gets to close and the
                // left line gets too far away, make a left turn.
            else if ((m_distToLeftMarking > 320 || m_distToLeftMarking < 0) &&
                     m_distToRightMarking < 230 && m_distToRightMarking > 0) {
                laneRecommendation.setAngle(-2.0);
                cerr << "Turning left! Left: " << m_distToLeftMarking << " & Right: " << m_distToRightMarking << endl;
            }
                // If all else fails, just keep going forward without turning
            else {
                laneRecommendation.setAngle(0);
                cerr << "Nothing." << endl;
            }

            // TimeStamp afterImageProcessing;
            // clog << "Processing time: " << (afterImageProcessing.toMicroseconds() - beforeProcessing.toMicroseconds()) / 1000.0 << "ms." << endl;

            // cerr << "Angle: " << desiredSteering << endl;

            if(panicStop) {
                laneRecommendation.setQuality(false);
            }

            else {
                laneRecommendation.setQuality(true);
                laneRecommendation.setDistance_to_line(0.0);
                laneRecommendation.setLeft_lane(false);
            }

            m_vehicleControl.setSteeringWheelAngle(laneRecommendation.getAngle());
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanedetector.debug") == 1;

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

                Container laneRecContainer(laneRecommendation);

                getConference().send(laneRecContainer);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    } // detector
} // lane
