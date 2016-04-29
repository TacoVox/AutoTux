#include <iostream>
#include <memory>
#include <math.h>

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
        const bool SIMMODE = false;

        LaneFollower::LaneFollower(const int32_t &argc, char **argv) :
                TimeTriggeredConferenceClientModule(argc, argv, "LaneFollower"),
                m_sharedImageMemory(),
                m_hasAttachedToSharedImageMemory(false),
                m_debug(false),
                m_image(),
                m_vehicleControl(),
                laneRecommendation(),
                overtaking(),
                config(),
                sensorBoardData(),
                m_previousTime(),
                m_eSum(0),
                m_eOld(0),
                distance(),
                control_scanline(),
                stop_scanline(),
                P_GAIN(),
                I_GAIN(),
                D_GAIN() {}

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
                I_GAIN = 0;
                D_GAIN = 0;
            }

            else {
                distance = CARDISTANCE;
                control_scanline = 462;
                stop_scanline = 350;

                P_GAIN = CARGAIN;
                I_GAIN = 0;
                D_GAIN = 0;
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

        uint8_t LaneFollower::getThreshold(double lightValue) {
            if(lightValue < 30.0)
                lightValue = 30.0;

            double returnVal = log10(lightValue) * 58.0 + lightValue * 0.16 - 30.0 + (pow((lightValue - 80.0) * 0.1, 3.0)) * 0.0034;

            return (uint8_t)returnVal;
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            // Copy the image to a matrix (this is the one we use for detection)
            Mat m_image_grey = m_image.clone();

            // Make the new image gray scale
            cvtColor(m_image_grey, m_image_grey, COLOR_BGR2GRAY);

            Canny(m_image_grey, m_image_grey, 50, 200, 3);

            imshow("ok", m_image_grey);

            /**
             * TODO Look into HoughLines to find edges.
             * Example below.
             */

            /*vector<Vec2f> contours;
            HoughLines(m_image_grey, contours, 1, CV_PI/180, 100, 0, 1);

            for(size_t i = 0; i < contours.size(); i++) {
                float rho = contours[i][0], theta = contours[i][1];
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line(m_image, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
            }*/

            // Find contours on the image
            vector<vector<Point>> contours;
            findContours(m_image_grey, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            // Draw the contours red
            for(size_t idx = 0; idx < contours.size(); idx++) {
                drawContours(m_image, contours, (int)idx, Scalar(0,0,255));
            }
        }

        double LaneFollower::laneDetection() {

            bool inLeftLane = overtaking.getLeftlane();

            double e = 0;

            // Lane detection loop
            for(int32_t y = m_image.rows - 8; y > m_image.rows * .5; y -= 10) {
                // Find red pixels
                Vec3b pixelLeft, pixelRight;
                Point left, right;

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

                // If the loop is currently checking at the height of our set control line
                if(y == control_scanline) {
                    // Right lane logic (prefer right line following)
                    if (!inLeftLane) {
                        if (right.x > 0) {
                            e = ((right.x - m_image.cols / 2.0) - distance) / distance;
                        } else if (left.x > 0) {
                            e = (distance - (m_image.cols / 2.0 - left.x)) / distance;
                        }
                    } else {
                        // Left lane logic (prefer left line following)
                        if (left.x > 0) {
                            e = (distance - (m_image.cols / 2.0 - left.x)) / distance;
                        } else if (right.x > 0) {
                            e = ((right.x - m_image.cols / 2.0) - distance) / distance;
                        }
                    }
                }

                // Draw debug lines
                if (m_debug) {

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

            Vec3b pixelFrontLeft, pixelFrontRight;
            Point stop_left, stop_right;

            int left_dist = 0;

            stop_left.x = (m_image.cols/2) - 50;
            stop_left.y = control_scanline;

            // Find first red pixel in front (stopline)
            for(int i = control_scanline; i > stop_scanline; i--) {
                pixelFrontLeft = m_image.at<Vec3b>(Point(stop_left.x, i));
                if(pixelFrontLeft.val[2] == 255) {
                    stop_left.y = i;
                    left_dist = control_scanline - stop_left.y;
                    break;
                }
            }

            int right_dist = 0;

            stop_right.x = (m_image.cols/2) + 50;
            stop_right.y = control_scanline;

            // Find first red pixel in front (stopline)
            for(int i = control_scanline; i > stop_scanline; i--) {
                pixelFrontRight = m_image.at<Vec3b>(Point(stop_right.x, i));
                if(pixelFrontRight.val[2] == 255) {
                    stop_right.y = i;
                    right_dist = control_scanline - stop_right.y;
                    break;
                }
            }

            if(m_debug) {
                if(stop_left.y < control_scanline) {
                    line(m_image, Point(stop_left.x, control_scanline), stop_left, Scalar(128, 0, 0));
                }

                if(stop_right.y < control_scanline) {
                    line(m_image, Point(stop_right.x, control_scanline), stop_right, Scalar(128, 0, 0));
                }
            }

            if((left_dist - right_dist > -5) && (left_dist - right_dist < 5)) {
                laneRecommendation.setDistance_to_line(left_dist);
            }

            return e;
        }

        void LaneFollower::laneFollowing(double e) {
            TimeStamp currentTime;
            double timeStep = (double)(currentTime.toMicroseconds() - m_previousTime.toMicroseconds()) / (1000.0 * 1000.0);
            m_previousTime = currentTime;

            if(fabs(e) < 1e-2) {
                m_eSum = 0;
            } else {
                m_eSum += e;
            }


            const double p = P_GAIN * e;
            const double i = I_GAIN * timeStep * m_eSum;
            const double d = D_GAIN * (e - m_eOld)/timeStep;

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

            // Limit max steering angle based on car limits
            if (desiredSteering > 0.5) desiredSteering = 0.5;
            if (desiredSteering < -0.5) desiredSteering = -0.5;

            if(laneRecommendation.getDistance_to_line() < 5 ||
               laneRecommendation.getDistance_to_line() > 150)
                laneRecommendation.setDistance_to_line(-1);

            //cout << "STOPLINE: " << laneRecommendation.getDistance_to_line() << endl;

            laneRecommendation.setAngle(desiredSteering);
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanefollower.debug") == 1;

            // ?
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                // For future reference if we decide on using light sensor data
                //Container sbd_container = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());

                Container image_container = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());
                Container config_container = getKeyValueDataStore().get(config::LaneFollowerMSG::ID());
                Container overtaking_container = getKeyValueDataStore().get(OvertakingMSG::ID());

                config = config_container.getData<config::LaneFollowerMSG>();
                overtaking = overtaking_container.getData<OvertakingMSG>();


                if (image_container.getDataType() == odcore::data::image::SharedImage::ID()) {
                    has_next_frame = readSharedImage(image_container);
                }

                if (has_next_frame) {
                    // For future reference if we decide on using light sensor data
                    //sensorBoardData = sbd_container.getData<automotive::miniature::SensorBoardData>();

                    processImage();
                    double detection = laneDetection();
                    laneFollowing(detection);
                }

                Container outContainer(laneRecommendation);
                getConference().send(outContainer);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    } // detector
} // lane
