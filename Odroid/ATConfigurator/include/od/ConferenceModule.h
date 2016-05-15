/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Time triggered class connecting to the OpenDaVINCI session for getting and
 * receiving data from it.
 */

#ifndef ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H
#define ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H

#include <opencv2/opencv.hpp>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <memory>

namespace od {
    class ConferenceModule
            : public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        ConferenceModule(const int32_t &argc, char **argv);
        virtual ~ConferenceModule();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
    private:
        ConferenceModule(const ConferenceModule &);
        ConferenceModule& operator=(const ConferenceModule &);
        virtual void setUp();
        virtual void tearDown();

        /**
        * Modified method (from Max' code) to fetch the image out of the shared memory.
        * After fetching it is stored in a jpg file which gets read out by our jp2a
        * executable to create an ASCII representation.
        */
        void readSharedImage(odcore::data::Container &);

        bool m_hasAttachedToSharedImageMemory;
        std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
        cv::Mat m_image;
    };
}

#endif //ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H
