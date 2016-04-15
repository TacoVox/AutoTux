#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include "serial/BufferWrapper.h"

namespace proxy {
    class Proxy :
            public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        Proxy(int32_t &argc, char **argv);
        Proxy(int32_t &argc, char **argv, std::shared_ptr<serial::BufferWrapper>);
        virtual ~Proxy();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        void interrupt(void);
        void setBufferWrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        std::shared_ptr<serial::BufferWrapper> bufferWrapper;
        bool interrupted;
        virtual void setUp();
        virtual void tearDown();
        std::vector<unsigned char> cdContToVec(odcore::data::Container);
        unsigned char checksum(std::vector<unsigned char>);
        void reconnect();
    };
}
#endif // SERIALHANDLER_H
