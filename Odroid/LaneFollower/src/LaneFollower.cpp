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

        // SET TO TRUE WHEN USING THE SIMULATOR
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
                control_scanline = 462;
                stop_scanline = 250;

                P_GAIN = SIMGAIN;
                E_GAIN = 0;
                I_GAIN = 0;
            }

            else {
                distance = CARDISTANCE;
                control_scanline = 462;
                stop_scanline = 350;

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
                    if(SIMMODE)
                    {
                        flip(m_image, m_image, -1);
                    }

                    returnValue = true;
                }
            }
            return returnValue;
        }

//        void LaneFollower::getLaneRecommendation(odcore::data::Container &c) {
//
//            // TODO: New datatype
//            if(c.getDataType() == LaneRecommendation::ID()) {
//
//                // TODO: New datatype
//                laneRecommendation = c.getData<LaneRecommendation>();
//            }
//        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            // TODO: New datatype
            laneRecommendation.setLeft_lane(false);
            inLeftLane = false;

            double e = 0;

            // Copy the image to a matrix (this is the one we use for detection)
            Mat m_image_grey = m_image.clone();

            // Make the new image grayscale
            cvtColor(m_image, m_image_grey, COLOR_BGR2GRAY);

            // Make the image binary, threshold set to 180 at the moment
            threshold(m_image_grey, m_image_grey, 180, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

            // Find contours on the image
            vector<vector<Point>> contours;
            findContours(m_image_grey, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            // Draw the contours red
            for(size_t idx = 0; idx < contours.size(); idx++) {
                drawContours(m_image, contours, (int)idx, Scalar(0,0,255));
            }

            // Lane detection loop
            for(int32_t y = m_image.rows - 8; y > m_image.rows * .5; y -= 10) {
                // Find red pixels
                Vec3b pixelLeft, pixelRight, pixelFront;
                Point left, right, middle;

                left.y = y;
                left.x = -1;

                // Find first red pixel to the left (left line)
                for (int x = m_image.cols / 2; x > 0; x--) {
                    pixelLeft = m_image.at<Vec3b>(Point(x, y));
                    if (pixelLeft.val[2] == 255) {
                        left.x = x;
                        break;
                    }
                }

                right.y = y;
                right.x = -1;

                // Find first red pixel to the right (right line)
                for (int x = m_image.cols / 2; x < m_image.cols; x++) {
                    pixelRight = m_image.at<Vec3b>(Point(x, y));
                    if (pixelRight.val[2] == 255) {
                        right.x = x;
                        break;
                    }
                }

                middle.x = (m_image.cols/2.0);
                middle.y = control_scanline;

                // Find first red pixel in front (stopline)
                for(int i = control_scanline; i > stop_scanline; i--) {
                    pixelFront = m_image.at<Vec3b>(Point(middle.x, i));
                    if(pixelFront.val[2] == 255) {
                        middle.y = i;
                        laneRecommendation.setDistance_to_line(control_scanline - middle.y);
                        break;
                    }
                }

                // If the loop is currently checking at the height of our set control line
                if(y == control_scanline) {

                    // Right lane logic (prefer right line following)
                    if (!inLeftLane) {
                        if (right.x > 0) {
                            e = ((right.x - m_image.cols / 2.0) - distance) / distance;
                        }

                        else if (left.x > 0) {
                            e = (distance - (m_image.cols / 2.0 - left.x)) / distance;
                        }
                    }

                    // Left lane logic (prefer left line following)
                    else {
                        if (left.x > 0) {
                            e = (distance - (m_image.cols / 2.0 - left.x)) / distance;
                        }

                        else if (right.x > 0) {
                            e = ((right.x - m_image.cols / 2.0) - distance) / distance;
                        }
                    }
                }

                // Draw debug lines
                if (m_debug) {

                    // Draw line from control line to stop line if there is one
                    if(middle.y < control_scanline) {
                        line(m_image, Point(middle.x, control_scanline), middle, Scalar(128, 0, 0));
                    }

                    // Draw lines from middle to the discovered left pixels
                    if (left.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), left, Scalar(0, 255, 0));
                        stringstream sstr;
                        sstr << (m_image.cols / 2 - left.x);
                        putText(m_image, sstr.str().c_str(), Point(m_image.cols / 2 - 100, y - 2), FONT_HERSHEY_PLAIN,
                                0.5, CV_RGB(0, 255, 0));
                    }

                    // Draw lines from middle to the discovered right pixels
                    if (right.x > 0) {
                        line(m_image, Point(m_image.cols / 2, y), right, Scalar(0, 128, 128));
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

            //cout << "DS: " << desiredSteering << endl;

            cout << "STOPLINE: " << laneRecommendation.getDistance_to_line() << endl;

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
                //Container laneContainer = getKeyValueDataStore().get(LaneRecommendation::ID());

                // TODO: New datatype
//                if(laneContainer.getDataType() == LaneRecommendation::ID()) {
//
//                    // TODO: New datatype
//                    getLaneRecommendation(laneContainer);
//                }

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
