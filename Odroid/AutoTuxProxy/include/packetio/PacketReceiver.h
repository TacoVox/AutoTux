//
// Created by Jonas Kahler on 4/1/16.
//

#ifndef SERIALCONNECTOR_PACKETRECEIVER_H
#define SERIALCONNECTOR_PACKETRECEIVER_H

#include <memory>
#include <opendavinci/odcore/io/PacketListener.h>
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include "serial/BufferWrapper.h"

namespace packetio {

    class PacketReceiver : public odcore::base::module::DataTriggeredConferenceClientModule {
    public:
        PacketReceiver(const int32_t &argc, char **argv);
        virtual ~PacketReceiver();
        virtual void nextContainer(odcore::data::Container &c);
        virtual void nextPacket(const odcore::io::Packet &p);
        void setBufferWrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        std::shared_ptr<serial::BufferWrapper> bufferWrapper;
        PacketReceiver(const PacketReceiver &/*obj*/);
        PacketReceiver& operator=(const PacketReceiver &/*obj*/);
        virtual void setUp();
        virtual void tearDown();
        unsigned char checksum(std::vector<unsigned char>);
    };

}

#endif //SERIALCONNECTOR_PACKETRECEIVER_H
