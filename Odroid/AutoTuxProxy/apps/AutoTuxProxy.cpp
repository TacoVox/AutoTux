//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <serial/SerialIOImpl.h>
#include "proxy/Proxy.h"
#include "serial/SerialHandler.h"

using namespace std;
using namespace serial;
using namespace proxy::camera;


// checks if verbose mode for the serial
bool verbose(int, char**);

int32_t main(int32_t argc, char **argv) {

    cout << "Starting up AutoTuxProxy..." << endl;

    // get verbose level
    bool is_verbose = verbose(argc, argv);

    // the serial buffer
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(is_verbose);

    // the serial connector
    shared_ptr<SerialIOInterface> sio =
            (shared_ptr<SerialIOInterface>) new SerialIOImpl();

    // the serial handler
    shared_ptr<SerialHandler> sh = (shared_ptr<SerialHandler>) new SerialHandler();
    sh->set_serialio(sio);
    sh->set_buffer(sb);
    sh->set_verbose(is_verbose);

    // thread for the serial handler
    thread uhthread(&SerialHandler::run, sh);
    uhthread.detach();

    proxy::Proxy proxy(argc, argv, sb);
    proxy.runModule();

    return 0;
}


bool verbose(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        string temp(argv[i]);
        if (temp == "--verbose=1") {
            return true;
        }
    }
    return false;
}

