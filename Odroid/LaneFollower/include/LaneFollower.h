#ifndef LANEFOLLOWER_H_
#define LANEFOLLOWER_H_

#include <opencv/cv.h>

#include <memory>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>

#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>

namespace lane {
    namespace follower {
        using namespace std;

        class LaneFollower : public odcore::base::module::TimeTriggeredConferenceClientModule {
        private:
            LaneFollower(const LaneFollower &/*obj*/);

            LaneFollower &operator=(const LaneFollower &/*obj*/);

        public:
            LaneFollower(const int32_t &argc, char **argv);

            virtual ~LaneFollower();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        protected:
            bool readSharedImage(odcore::data::Container &c);

        private:
            bool m_hasAttachedToSharedImageMemory;
            std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
            IplImage *m_image;

            bool m_debug;
            CvFont m_font;

            odcore::data::TimeStamp m_previousTime;
            double m_eSum;
            double m_eOld;

            automotive::VehicleControl m_vehicleControl;

            virtual void setUp();

            virtual void tearDown();

            void processImage();
        };
    } // follower
} // lane

#endif