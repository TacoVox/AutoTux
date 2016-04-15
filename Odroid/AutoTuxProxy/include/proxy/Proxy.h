#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odtools/recorder/Recorder.h>

#include "camera/Camera.h"
#include "serial/BufferWrapper.h"

namespace proxy {
    using namespace std;

    class Proxy : public odcore::base::module::TimeTriggeredConferenceClientModule {
    private:
        Proxy(int32_t &argc, char **argv);
        Proxy(const Proxy &/*obj*/);
        Proxy& operator=(const Proxy &/*obj*/);

    public:
        Proxy(int32_t &argc, char **argv, shared_ptr<serial::BufferWrapper>);

        virtual ~Proxy();

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        void interrupt(void);
        void setBufferWrapper(shared_ptr<serial::BufferWrapper>);

    private:
        virtual void setUp();
        virtual void tearDown();

        void reconnect();
        void distribute(odcore::data::Container c);

    private:
        unique_ptr<odtools::recorder::Recorder> m_recorder;
        unique_ptr<proxy::camera::Camera> m_camera;
        shared_ptr<serial::BufferWrapper> bufferWrapper;
        vector<unsigned char> cdContToVec(odcore::data::Container);

        bool interrupted;
        unsigned char checksum(std::vector<unsigned char>);
    };
}
#endif // SERIALHANDLER_H
