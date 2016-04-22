#ifndef LANEDETECTOR_H_
#define LANEDETECTOR_H_

#define SIMGAIN 2.6;
#define SIMDISTANCE 280;

#define CARGAIN 1.7;
#define CARDISTANCE 220;

#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

namespace lane {
    namespace follower {
        using namespace std;

        class LaneFollower: public odcore::base::module::TimeTriggeredConferenceClientModule {
        private:
            /**
             * Forbidden.
             *
             * TODO Describe why
             *
             * @param obj References an object of this class.
             */
            LaneFollower(const LaneFollower &/*obj*/);

            /**
             * Forbidden
             *
             * TODO Describe why
             *
             * @param obj References an object of this class.
             * @return Reference to this instance.
             */
            LaneFollower& operator=(const LaneFollower &/*obj*/);

        public:
            /**
             * Main constructor
             */
            LaneFollower(const int32_t &argc, char **argv);

            virtual ~LaneFollower();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        protected:
            /**
             * This method is called to process incoming containers
             *
             * @param c Container to process.
             * @return true if successfully processed.
             */
            bool readSharedImage(odcore::data::Container &c);
            int m_distToLeftMarking;
            int m_distToRightMarking;

        private:
            bool m_hasAttachedToSharedImageMemory;
            bool m_debug;
            bool panicStop;
            double m_eSum;
            double m_eOld;

            int32_t distance;
            int32_t control_scanline;
            int32_t stop_scanline;

            // For introduction to algorithm see
            // https://www  .youtube.com/watch?v=4Y7zG48uHRo
            // Proportional gain. Values above 1 amplifies e and vice versa.
            // 1 too low for right curve, 4 too twitchy. 2-3 seems very good
            double P_GAIN;
            // Cross track error rate gain. Affects the angle based on how fast we
            // are moving towards the desired center of the lane. Counters the primary
            // proportional correction. Increase if car wobbles around centerline
            // because of of overcorrection.
            double E_GAIN;
            // Integral gain. Adjusts based on accumulated e values, to correct for
            // offset.
            double I_GAIN;

            bool inLeftLane;

            std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
            cv::Mat m_image;
            odcore::data::TimeStamp m_previousTime;

            automotive::VehicleControl m_vehicleControl;

            autotux::LaneRecommendation laneRecommendation;

            virtual void setUp();
            virtual void tearDown();

            void processImage();
            void getLaneRecommendation(odcore::data::Container &c);
        };
    } // follower
} // lane


#endif
