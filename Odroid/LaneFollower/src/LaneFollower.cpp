#include "LaneFollower.h"

// Used for debugging
#include <opendavinci/odtools/player/Player.h>

namespace lane {
    namespace follower {
        using namespace std;
        using namespace cv;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::data::dmcp;
        using namespace odcore::data::image;
        using namespace odcore::wrapper;
        using namespace autotux;
        using namespace autotux::config;

        using namespace lane::follower;

        // Debugging use only
        using namespace odtools::player;

        // SET TO TRUE WHEN USING THE SIMULATOR
        // Do we want this in the configuration file?
        const bool SIMMODE = false;
		
		Mat m_image_grey; 
		TimeStamp startTime, endTime;

        TimeStamp configContainerTimeStamp = TimeStamp();

        LaneFollower::LaneFollower(const int32_t &argc, char **argv) :
                TimeTriggeredConferenceClientModule(argc, argv, "LaneFollower"),
                m_sharedImageMemory(),
                m_sharedProcessedImageMemory(),
                m_sharedProcessedImage(),
                m_hasAttachedToSharedImageMemory(false),
                m_debug(false),
                m_image(),
                m_vehicleControl(),
                m_laneRecommendation(),
                m_overtaking(),
                m_config(),
                m_sensorBoardData(),
                m_previousTime(),
                m_eSum(0),
                m_eOld(0),
                m_distance(190),
                m_controlScanline(222),
                m_stopScanline(10),
                m_threshold1(50),
                m_threshold2(200),
                P_GAIN(0.80),
                I_GAIN(0.0),
                D_GAIN(0.0),
                printCounter(0) {}

        LaneFollower::~LaneFollower() { }

        // This method will run before body()
        void LaneFollower::setUp() {
            // Set up debug window
            if (m_debug) {
                cvNamedWindow("Debug Window", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("Debug Window", 300, 100);
            }

            if(SIMMODE) {
                m_distance = SIMDISTANCE;
                m_controlScanline = 462;
                m_stopScanline = 250;

                P_GAIN = SIMGAIN;
                I_GAIN = 0;
                D_GAIN = 0;
            }
        }

        // This method will run after return from body()
        void LaneFollower::tearDown() {
            if (!m_image.empty()) {
                m_image.deallocate();
            }

            if (m_debug) {
                cvvDestroyWindow("Debug Window");
            }
        }

        bool LaneFollower::readSharedImage(Container &c) {
            bool returnValue = false;

            if (c.getDataType() == SharedImage::ID()) {
                SharedImage si = c.getData<SharedImage>();
                if (si.getName() == "WebCam") { // Make this read from configuration file as the proxy does?
                    // Have we already attached to the shared memory containing the image?
                    if (!m_hasAttachedToSharedImageMemory) {
                        m_sharedImageMemory = SharedMemoryFactory::attachToSharedMemory(si.getName());

                        // Set processed image things
                        m_sharedProcessedImageMemory = SharedMemoryFactory::createSharedMemory("ProcessedImage",
                                                                                               si.getHeight() *
                                                                                               si.getWidth());
                        m_sharedProcessedImage.setName("ProcessedImage");
                        m_sharedProcessedImage.setWidth(si.getWidth());
                        m_sharedProcessedImage.setHeight(si.getHeight());
                        m_sharedProcessedImage.setBytesPerPixel(1);
                        m_sharedProcessedImage.setSize(si.getWidth() * si.getHeight());

                        // We have now attached to the shared image memory.
                        m_hasAttachedToSharedImageMemory = true;
                    }

                    // Did we successfully connect?
                    if (m_sharedImageMemory->isValid()) {
                        m_sharedImageMemory->lock();
                        // Create image(cv::Mat) if empty.
                        if (m_image.empty()) {
                            m_image.create(si.getHeight(), si.getWidth(), CV_8UC3);
                        } else {
                            // Copy image data form SharedImageMemory
                            memcpy(m_image.data, m_sharedImageMemory->getSharedMemory(),
                                   si.getHeight() * si.getWidth() * si.getBytesPerPixel());
                        }
                        m_sharedImageMemory->unlock();

                        // Mirror image
                        // NOTE: For simulator.
                        if (SIMMODE) {
                            flip(m_image, m_image, -1);
                        }

                        returnValue = true;
                    }
                }
            }
            return returnValue;
        }

        // Do magic to the image around here.
        void LaneFollower::processImage() {

            // Copy the image to a matrix (this is the one we use for detection)
            m_image_grey = m_image.clone();

            // Make the new image gray scale
            cvtColor(m_image_grey, m_image_grey, COLOR_BGR2GRAY);

            Canny(m_image_grey, m_image_grey, m_threshold1, m_threshold2, 3);
			/*
     	    if(m_sharedProcessedImageMemory.get() && m_sharedProcessedImageMemory->isValid()) {
                m_sharedProcessedImageMemory->lock();
                memcpy(m_sharedProcessedImageMemory->getSharedMemory(), m_image_grey.data, 640*240); // Set size dynamically?
                m_sharedProcessedImageMemory->unlock();
            }*/

            /**
             * TODO Look into Hough Lines to find edges.
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
            //vector<vector<Point>> contours;
            //findContours(m_image_grey, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            // Draw the contours red
            //for(size_t idx = 0; idx < contours.size(); idx++) {
            //    drawContours(m_image, contours, (int)idx, Scalar(0,0,255));
            //}
        }

        double LaneFollower::laneDetection() {

            bool inLeftLane = m_overtaking.getLeftlane();

            double e = 0;

            // Lane detection loop
            for(int32_t y = m_image_grey.rows - 8; y > m_image_grey.rows * .5; y -= 10) {
                // Find red pixels
                unsigned char pixelLeft, pixelRight;
                Point left, right;

                left.y = y;
                left.x = -1;

                // Find first red pixel to the left (left line)
                for (int x = m_image_grey.cols / 2; x > 0; x--) {
                    pixelLeft = m_image_grey.at<unsigned char>(Point(x, y));
                    if (pixelLeft == 150) {
                        left.x = x;
                        break;
                    }
                }

                right.y = y;
                right.x = -1;

                // Find first red pixel to the right (right line)
                for (int x = m_image_grey.cols / 2; x < m_image_grey.cols; x++) {
                    pixelRight = m_image_grey.at<unsigned char>(Point(x, y));
                    if (pixelRight > 150) {
                        right.x = x;
                        break;
                    }
                }

                // If the loop is currently checking at the height of our set control line
                if(y == m_controlScanline) {
                    // Quality check, if no pixels are detected on either side, set quality to false
                    if(right.x < 0 && left.x < 0) {
                        m_laneRecommendation.setQuality(false);
                    }

                    // Otherwise the quality is fine
                    else {
                        m_laneRecommendation.setQuality(true);
                    }

                    // Right lane logic (prefer right line following)
                    if (!inLeftLane) {
                        if (right.x > 0) {
                            e = ((right.x - m_image_grey.cols / 2.0) - m_distance) / m_distance;
                        } else if (left.x > 0) {
                            e = (m_distance - (m_image_grey.cols / 2.0 - left.x)) / m_distance;
                        }
                    } else {
                        // Left lane logic (prefer left line following)
                        if (left.x > 0) {
                            e = (m_distance - (m_image_grey.cols / 2.0 - left.x)) / m_distance;
                        } else if (right.x > 0) {
                            e = ((right.x - m_image_grey.cols / 2.0) - m_distance) / m_distance;
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
            } // for loop

            unsigned char pixelFrontLeft, pixelFrontRight;
            Point stop_left, stop_right;

            int left_dist = 0;

            stop_left.x = (m_image_grey.cols/2) - 50;
            stop_left.y = m_controlScanline;

            // Find first red pixel in front (stopline)
            for(int i = m_controlScanline; i > m_stopScanline; i--) {
                pixelFrontLeft = m_image_grey.at<unsigned char>(Point(stop_left.x, i));
                if(pixelFrontLeft > 150) {
                    stop_left.y = i;
                    left_dist = m_controlScanline - stop_left.y;
                    break;
                }
            }

            int right_dist = 0;

            stop_right.x = (m_image_grey.cols/2) + 50;
            stop_right.y = m_controlScanline;

            // Find first red pixel in front (stopline)
            for(int i = m_controlScanline; i > m_stopScanline; i--) {
                pixelFrontRight = m_image_grey.at<unsigned char>(Point(stop_right.x, i));
                if(pixelFrontRight > 150) {
                    stop_right.y = i;
                    right_dist = m_controlScanline - stop_right.y;
                    break;
                }
            }

            if(m_debug) {
                if(stop_left.y < m_controlScanline) {
                    line(m_image, Point(stop_left.x, m_controlScanline), stop_left, Scalar(128, 0, 0));
                }

                if(stop_right.y < m_controlScanline) {
                    line(m_image, Point(stop_right.x, m_controlScanline), stop_right, Scalar(128, 0, 0));
                }
            }

            if((left_dist - right_dist > -10) && (left_dist - right_dist < 10)) {
                m_laneRecommendation.setDistance_to_line(left_dist);
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

            if(m_laneRecommendation.getDistance_to_line() < 10 || m_laneRecommendation.getDistance_to_line() > 50)
                // Set distance to line to -1 if it's too far away or too close
                m_laneRecommendation.setDistance_to_line(-1);

            m_laneRecommendation.setAngle(desiredSteering);
        }

        /**
         * Function that prints debug output every second instead of every iteration.
         */
        void LaneFollower::printDebug() {
            if(printCounter == 25) {

                // Print values sent through to the DM
                cout << "STOPLINE: " << m_laneRecommendation.getDistance_to_line() << endl;
                cout << "DESIRED STEERING: " << m_laneRecommendation.getAngle() << endl;
                cout << "QUALITY: " << m_laneRecommendation.getQuality() << endl;
                cout << "IN LEFT LANE: " << m_overtaking.getLeftlane() << endl;
           		cout << "TIME IN BODY: " << (endTime.toMicroseconds() - startTime.toMicroseconds()) << endl;
			   	cout << "-----------------------------------" << endl;

                // Reset counter
                printCounter = 0;
            }
            else {
                printCounter++;
            }
        }

        ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t>("lanefollower.debug") == 1;

            // ?
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == ModuleStateMessage::RUNNING) {
                startTime = TimeStamp();
				bool has_next_frame = false;

                Container image_container = getKeyValueDataStore().get(SharedImage::ID());
               
                Container config_container = getKeyValueDataStore().get(LaneFollowerMSG::ID());
                Container overtaking_container = getKeyValueDataStore().get(OvertakingMSG::ID());
				//cout << "ts: "<< config_container.getReceivedTimeStamp() <<endl;


                // TODO We are receiving values all the time.
                // Implement check if changed.
                if(config_container.getReceivedTimeStamp() > configContainerTimeStamp) {
                    m_config = config_container.getData<LaneFollowerMSG>();
					if (m_config.getThresholdD() > 0) {
                    	m_threshold1 = m_config.getThresholdD();
                    	m_threshold2 = m_config.getThresholdB();
                    	m_distance = m_config.getRoadWidth();

                    	P_GAIN = m_config.getGainP();
                    	I_GAIN = m_config.getGainI();
                    	D_GAIN = m_config.getGainD();

                    	configContainerTimeStamp = TimeStamp();

                    	LaneFollower::toLogger(LogMessage::DEBUG, m_config.toString());
                	}
				}
                m_overtaking = overtaking_container.getData<OvertakingMSG>();


                if (image_container.getDataType() == SharedImage::ID()) {
                    has_next_frame = readSharedImage(image_container);
                }

                if (has_next_frame) {
                    processImage();
                    double detection = laneDetection();
                    laneFollowing(detection);
                }
                Container laneRecommendationContainer(m_laneRecommendation);
                //Container processedImageContainer(m_sharedProcessedImage);
                //getConference().send(processedImageContainer);
                getConference().send(laneRecommendationContainer);
				endTime = TimeStamp();
                printDebug();
            }
            return ModuleExitCodeMessage::OKAY;
        }
    } // detector
} // lane
