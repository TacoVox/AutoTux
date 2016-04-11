
//
// Created by Jonas Kahler on 4/1/16.
//

#ifndef SERIALCONNECTOR_PACKETRECEIVER_H
#define SERIALCONNECTOR_PACKETRECEIVER_H

#include <opendavinci/odcore/io/PacketListener.h>
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>

namespace packetio {

    class PacketReceiver : public odcore::base::module::DataTriggeredConferenceClientModule {
    public:
        PacketReceiver(const int32_t &argc, char **argv);
        virtual ~PacketReceiver();
        virtual void nextContainer(odcore::data::Container &c);
    private:
        PacketReceiver(const PacketReceiver &/*obj*/);
        PacketReceiver& operator=(const PacketReceiver &/*obj*/);
        virtual void setUp();
        virtual void tearDown();
    };

}

#endif //SERIALCONNECTOR_PACKETRECEIVER_H
