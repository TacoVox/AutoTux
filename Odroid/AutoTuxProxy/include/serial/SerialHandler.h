#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include "serial/USBConnector.h"
#include "serial/BufferWrapper.h"

namespace serial {
    class SerialHandler :
            public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        SerialHandler(int32_t &argc, char **argv);
        virtual ~SerialHandler();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        void interrupt(void);
    private:
        bool interrupted;
        std::shared_ptr<usb_connector::USBConnector> usbConnector;
        std::shared_ptr<serial::BufferWrapper> bufferWrapper;
        virtual void setUp();
        virtual void tearDown();
        unsigned char checksum(std::vector<unsigned char>);
    };
}
#endif // SERIALHANDLER_H
