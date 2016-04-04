//
// Created by Jonas Kahler on 4/1/16.
//

#include "PacketReceiver.h"

#include <thread>
#include <iostream>
#include <typeinfo>
#include <opendavinci/odcore/base/Thread.h>
#include <opendavinci/odcore/io/Packet.h>
#include <opendavinci/odcore/io/udp/UDPReceiver.h>
#include <opendavinci/odcore/io/udp/UDPFactory.h>

using namespace std;
using namespace odcore;
using namespace odcore::io;
using namespace odcore::io::udp;


void packetio::PacketReceiver::test(void) {
    //Do the stuff 100 times
    int i;
    for(i = 0; i < 100; i++) {
        receive();
    }
}

void packetio::PacketReceiver::receive(void) {
    const string RECEIVER = "127.0.0.1";
    const uint32_t PORT = 1111;

    // We are using OpenDaVINCI's std::shared_ptr to automatically
    // release any acquired resources.
    try {
        std::shared_ptr<UDPReceiver>
                udpreceiver(UDPFactory::createUDPReceiver(RECEIVER, PORT));

        // This instance will handle any packets that are received
        // by our UDP socket.
        PacketReceiver handler;
        udpreceiver->setPacketListener(&handler);

        // Start receiving bytes.
        udpreceiver->start();

        const uint32_t ONE_SECOND = 1000 * 1000;
        odcore::base::Thread::usleepFor(10 * ONE_SECOND);

        // Stop receiving bytes and unregister our handler.
        udpreceiver->stop();
        udpreceiver->setPacketListener(NULL);
    }
    catch(string &exception) {
        cerr << "Error while creating UDP receiver: " << exception << endl;
    }
}

void packetio::PacketReceiver::nextPacket(const odcore::io::Packet &packet) {
    //Simple output for now
    cout << packet.getData() << endl;
}