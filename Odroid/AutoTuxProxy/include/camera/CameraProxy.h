#ifndef CAMERAPROXY_H_
#define CAMERAPROXY_H_

#include <map>
#include <memory>

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odtools/recorder/Recorder.h>

#include "Camera.h"

namespace proxy {
    namespace camera {
        using namespace std;

        /**
         * This class wraps the software/hardware interface.
         */
        class CameraProxy : public odcore::base::module::TimeTriggeredConferenceClientModule {
        private:
            /**
             * Forbidden copy constructor
             */
            CameraProxy(const CameraProxy &/*obj*/);

            /**
             * Forbidden assignment operator.
             */
            CameraProxy& operator=(const CameraProxy &/*obj*/);

        public:
            /**
             * Main constructor.
             *
             * @param argc No. command line arguments.
             * @param argv Command line arguments.
             */
            CameraProxy(const int32_t &argc, char **argv);

            virtual ~CameraProxy();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        private:
            virtual void setUp();
            virtual void tearDown();

            void distribute(odcore::data::Container c);

        private:
            unique_ptr<odtools::recorder::Recorder> m_recorder;
            unique_ptr<Camera> m_camera;
        };
    }
}

#endif