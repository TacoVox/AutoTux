#ifndef LANEDETECTOR_H_
#define LANEDETECTOR_H_

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
            std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
            cv::Mat m_image;
            odcore::data::TimeStamp m_previousTime;
            bool m_debug;

            automotive::VehicleControl m_vehicleControl;

            autotux::LaneRecommendation laneRecommendation;

            bool panicStop;

            virtual void setUp();
            virtual void tearDown();

            void processImage();
        };
    } // follower
} // lane


#endif
