//
// Created by Jonas Kahler on 4/1/16.
//

#ifndef SERIALCONNECTOR_PACKETRECEIVER_H
#define SERIALCONNECTOR_PACKETRECEIVER_H

#include <opendavinci/odcore/io/PacketListener.h>

namespace packetio {

    class PacketReceiver : public odcore::io::PacketListener {
    public:
        void test(void);
        void receive(void);

    private:
        virtual void nextPacket(const odcore::io::Packet &packet);
    };

}

#endif //SERIALCONNECTOR_PACKETRECEIVER_H
