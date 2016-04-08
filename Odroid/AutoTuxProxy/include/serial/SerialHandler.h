#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <cstdint>
#include <memory>
#include "packetio/PacketBroadcaster.h"
#include "packetio/PacketReceiver.h"

namespace serial {
    class SerialHandler {
    public:
        SerialHandler(int32_t &argc, char **argv);
        void run(void);
    private:
        std::shared_ptr<packetio::PacketBroadcaster> packetBroadcaster;
        std::shared_ptr<packetio::PacketReceiver> packetReceiver;
    };
}
#endif // SERIALHANDLER_H