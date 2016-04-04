//
// Created by Jonas Kahler on 4/1/16.
//

#include "PacketBroadcaster.h"
#include <stdint.h>
#include <iostream>
#include <memory>
#include <opendavinci/odcore/io/udp/UDPSender.h>
#include <opendavinci/odcore/io/udp/UDPFactory.h>

using namespace std;
using namespace odcore;
using namespace odcore::io::udp;

void packetio::UDPBroadcaster::sendtest(void) {
    //Send 100 times some shit
    int i;
    for(i = 0; i < 100; i++) {
        send();
    }
}

void packetio::UDPBroadcaster::send(void) {
    const string RECEIVER = "127.0.0.1";
    const uint32_t PORT = 1111;

    // We are using OpenDaVINCI's std::shared_ptr to automatically
    // release any acquired resources.
    try {
        std::shared_ptr<UDPSender> udpsender(UDPFactory::createUDPSender(RECEIVER, PORT));
        cout << "Sending data...\n";
        udpsender->send("Hello World\r\n");
    }
    catch(string &exception) {
        cerr << "Data could not be sent: " << exception << endl;
    }
}



