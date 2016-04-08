#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include "packetio/PacketBroadcaster.h"
#include "packetio/PacketReceiver.h"
#include "serial/USBConnector.h"

namespace serial {
    class SerialHandler {
    public:
        SerialHandler(int32_t &argc, char **argv);
        void run(void);
    private:
        std::shared_ptr<packetio::PacketBroadcaster> packetBroadcaster;
        std::shared_ptr<packetio::PacketReceiver> packetReceiver;
        std::shared_ptr<usb_connector::USBConnector> usbConnector;
    };
}
#endif // SERIALHANDLER_H
