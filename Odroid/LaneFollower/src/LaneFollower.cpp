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
                laneRecommendation(),
                m_eSum(0),
                m_eOld(0) {}

        LaneFollower::~LaneFollower() { }

        // This method will run before body()
        void LaneFollower::setUp() {
            // Set up debug window
            cout << "Setup LaneFollower" << endl;
            cout << "LaneFollower Debug: " << m_debug << endl;
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
                    // flip(m_image, m_image, -1);
                    returnValue = true;
                }
            }
            return returnValue;
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {
            static bool useLeftMarking = true;
            double e = 0;

            const int32_t CONTROL_SCANLINE = 462;
            const int32_t distance = 220;

            Mat m_image_grey = m_image.clone();
            cvtColor(m_image, m_image_grey, COLOR_BGR2GRAY);
            threshold(m_image_grey, m_image_grey, 180, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

            vector<vector<Point>> contours;
            findContours(m_image_grey, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            for(size_t idx = 0; idx < contours.size(); idx++) {
                drawContours(m_image, contours, (int)idx, Scalar(0,0,255));
            }

            for(int32_t y = m_image.rows - 8; y > m_image.rows * .5; y -= 10) {
                // Find red pixels
                Vec3b pixelLeft, pixelRight;
                Point left, right;

                left.y = y;
                left.x = -1;

                for (int x = m_image.cols / 2; x > 0; x--) {
                    pixelLeft = m_image.at<Vec3b>(Point(x, y));
                    if (pixelLeft.val[2] == 255) {
                        left.x = x;
                        break;
                    }
                }

                right.y = y;
                right.x = -1;
                for (int x = m_image.cols / 2; x < m_image.cols; x++) {
                    pixelRight = m_image.at<Vec3b>(Point(x, y));
                    if (pixelRight.val[2] == 255) {
                        right.x = x;
                        break;
                    }
                }

                if(y == CONTROL_SCANLINE) {
                    // Testing Twiddle algorithm stuff
                    if(right.x > 0) {
                        if(!useLeftMarking) {
                            //m_eSum = 0;
                            //m_eOld = 0;
                        }

                        e = ((right.x - m_image.cols/2.0) - distance) / distance;
                        useLeftMarking = true;

                    } else if(left.x > 0) {
                        if(useLeftMarking) {
                            //m_eSum = 0;
                            //m_eOld = 0;
                        }

                        e = (distance - (m_image.cols/2.0 - left.x)) / distance;
                        useLeftMarking = false;

                    } else {
                        //m_eSum = 0;
                        //m_eOld = 0;
                    }
                }

                if (m_debug) {
                    if (left.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), left, Scalar(0, 255, 0));
                        stringstream sstr;
                        sstr << (m_image.cols / 2 - left.x);
                        putText(m_image, sstr.str().c_str(), Point(m_image.cols / 2 - 100, y - 2), FONT_HERSHEY_PLAIN,
                                0.5, CV_RGB(0, 255, 0));
                    }
                    if (right.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), right, Scalar(0, 0, 255));
                        stringstream sstr;
                        sstr << (right.x - m_image.cols / 2);
                        putText(m_image, sstr.str().c_str(), Point(m_image.cols / 2 + 100, y - 2), FONT_HERSHEY_PLAIN,
                                0.5, CV_RGB(255, 0, 0));
                    }
                }
            }

            TimeStamp currentTime;
            double timeStep = (currentTime.toMicroseconds() - m_previousTime.toMicroseconds()) / (1000.0 * 1000.0);
            m_previousTime = currentTime;

            if(fabs(e) < 1e-2) {
                m_eSum = 0;
            } else {
                m_eSum += e;
            }

	
            const double Kp = 1.7;

            // For introduction to algorithm see
            // https://www  .youtube.com/watch?v=4Y7zG48uHRo
            // Proportional gain. Values above 1 amplifies e and vice versa.
            // 1 too low for right curve, 4 too twitchy. 2-3 seems very good
            // Cross track error rate gain. Affects the angle based on how fast we
            // are moving towards the desired center of the lane. Counters the primary
            // proportional correction. Increase if car wobbles around centerline
            // because of of overcorrection.
            const double Kd = 0.0;
            // Integral gain. Adjusts based on accumulated e values, to correct for
            // offset.
            const double Ki = 0;


            const double p = Kp * e;
            const double i = Ki * timeStep * m_eSum;
            const double d = Kd * (e - m_eOld)/timeStep;
            m_eOld = e;

            const double y = p + i + d;

            double desiredSteering = 0;
            if(fabs(e) > 1e-2) {
                desiredSteering = y;
            }

            // Make this use debug flag when finished? Or remove?
            if (false) {
                if (m_image.data != NULL) {
                    imshow("Camera Original Image", m_image);
                    imshow("Camera BW Image", m_image_grey);
                    waitKey(10);
                }
            }

            // Limit max steering anlge based on car limits
            if (desiredSteering > 0.5) desiredSteering = 0.5;
            if (desiredSteering < -0.5) desiredSteering = -0.5;

            cout << "DS: " << desiredSteering;
            laneRecommendation.setAngle(desiredSteering);
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanedetector.debug") == 1;

            // ?
            cout << "Entering loop:" << endl;
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

                if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                    cout << "Read shared image";
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
