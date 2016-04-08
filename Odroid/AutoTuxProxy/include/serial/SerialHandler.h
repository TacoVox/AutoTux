#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include "packetio/PacketBroadcaster.h"
#include "packetio/PacketReceiver.h"
#include "serial/USBConnector.h"
#include "serial/BufferWrapper.h"

namespace serial {
    class SerialHandler {
    public:
        SerialHandler(int32_t &argc, char **argv);
        ~SerialHandler();
        void run(void);
    private:
        std::shared_ptr<packetio::PacketBroadcaster> packetBroadcaster;
        std::shared_ptr<packetio::PacketReceiver> packetReceiver;
        std::shared_ptr<usb_connector::USBConnector> usbConnector;
        std::shared_ptr<serial::BufferWrapper> bufferWrapper;
    };
}
#endif // SERIALHANDLER_H
