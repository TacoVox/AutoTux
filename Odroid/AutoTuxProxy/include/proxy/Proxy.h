#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include "serial/USBConnector.h"
#include "serial/BufferWrapper.h"

namespace proxy {
    class ProxyMaster :
            public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        ProxyMaster(int32_t &argc, char **argv);
        virtual ~ProxyMaster();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        void interrupt(void);
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
