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
        using namespace autotux;

        using namespace lane::follower;

        // Debugging use only
        using namespace odtools::player;

        // SET TO TRUE FOR CARMODE
        const bool SIMMODE = true;

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
            if (m_debug) {
                cvNamedWindow("Debug Window", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("Debug Window", 300, 100);
            }

            if(SIMMODE) {
                distance = SIMDISTANCE;
                scanline = 462;

                P_GAIN = SIMGAIN;
                E_GAIN = 0;
                I_GAIN = 0;
            }

            else {
                distance = CARDISTANCE;
                scanline = 462;

                P_GAIN = CARGAIN;
                E_GAIN = 0;
                I_GAIN = 0;
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
                    if(SIMMODE) {
                        flip(m_image, m_image, -1);
                    }
                    returnValue = true;
                }
            }
            return returnValue;
        }

        // TODO: New datatype
        void LaneFollower::getLaneRecommendation(odcore::data::Container &c) {

            // TODO: New datatype
            if(c.getDataType() == LaneRecommendation::ID()) {

                // TODO: New datatype
                laneRecommendation = c.getData<LaneRecommendation>();
            }
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            // TODO: New datatype
            laneRecommendation.setLeft_lane(false);
            inLeftLane = laneRecommendation.getLeft_lane();

            double e = 0;

            Mat m_image_grey = m_image.clone();
            cvtColor(m_image, m_image_grey, COLOR_BGR2GRAY);
            threshold(m_image_grey, m_image_grey, 180, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

            //adaptiveThreshold(m_image_grey, m_image_grey, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 5);

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

                if(y == scanline) {
                    // Testing Twiddle algorithm stuff
                    if(!inLeftLane) {
                        e = ((right.x - m_image.cols/2.0) - distance) / distance;
                    }

                    else {
                        e = ((left.x - m_image.cols/2.0) - distance) / distance;
                    }
                }

                if (m_debug) {

                    //line(m_image, Point(m_image.cols / 2, 0), Point(m_image.cols / 2, m_image.rows), Scalar(0,255,255));
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

            const double p = P_GAIN * e;
            const double i = I_GAIN * timeStep * m_eSum;
            const double d = E_GAIN * (e - m_eOld)/timeStep;
            m_eOld = e;

            const double y = p + i + d;

            double desiredSteering = 0;
            if(fabs(e) > 1e-2) {	
                desiredSteering = y;
            }

            if (m_debug) {
                if (m_image.data != NULL) {
                    imshow("Camera Original Image", m_image);
                    waitKey(10);
                }
            }

            // Limit max steering anlge based on car limits
            if (desiredSteering > 0.5) desiredSteering = 0.5;
            if (desiredSteering < -0.5) desiredSteering = -0.5;

            cout << "\rDS: " << desiredSteering;
            laneRecommendation.setAngle(desiredSteering);
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanedetector.debug") == 1;

            // ?
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                Container imageContainer = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

                // TODO: New datatype
                Container laneContainer = getKeyValueDataStore().get(LaneRecommendation::ID());

                // TODO: New datatype
                if(laneContainer.getDataType() == LaneRecommendation::ID()) {

                    // TODO: New datatype
                    getLaneRecommendation(laneContainer);
                }

                if (imageContainer.getDataType() == odcore::data::image::SharedImage::ID()) {
                    has_next_frame = readSharedImage(imageContainer);
                }

                if (has_next_frame) {
                    processImage();
                }

                Container outContainer(laneRecommendation);
                getConference().send(outContainer);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    } // detector
} // lane
