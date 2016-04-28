#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#define MATH_PI  3.1415926535897

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
        //Constructors & Destructor
        Proxy(int32_t &argc, char **argv);
        Proxy(const Proxy &/*obj*/);
        Proxy& operator=(const Proxy &/*obj*/);
        //Member functions
        virtual void setUp();
        virtual void tearDown();
        void reconnect();
        void distribute(odcore::data::Container c);
        vector<unsigned char> contToVec(odcore::data::Container, odcore::data::Container);
        unsigned char checksum(std::vector<unsigned char>);
        //Private fields
        bool interrupted;
        shared_ptr<serial::BufferWrapper> bufferWrapper;
        unique_ptr<odtools::recorder::Recorder> m_recorder;
        unique_ptr<proxy::camera::Camera> m_camera;

    public:
        Proxy(int32_t &argc, char **argv, shared_ptr<serial::BufferWrapper>);
        virtual ~Proxy();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        void interrupt(void);
        void setBufferWrapper(shared_ptr<serial::BufferWrapper>);
    };
}
#endif // SERIALHANDLER_H
