//
// Created by jonas on 4/1/16.
//
#include <iostream>
#include <thread>
#include "serial/SerialHandler.h"


using namespace std;
using namespace serial;


int main(int argc, char **argv) {
    cout << "Starting up AutoTuxProxy..." << endl;

    //Setup the serialhandler
    shared_ptr<SerialHandler> serialHandler(new SerialHandler(argc, argv));
    thread shthread(&SerialHandler::run, serialHandler);

    //Waiting for the thread to terminate
    shthread.join();

    return 0;
}
