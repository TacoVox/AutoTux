//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <csignal>
#include <serial/SerialIOImpl.h>
#include "proxy/Proxy.h"
#include "serial/SerialHandler.h"

using namespace std;
using namespace serial;
using namespace proxy::camera;


//static void handler(int);
bool verbose(int, char**);

int32_t main(int32_t argc, char **argv) {

    //signal(SIGINT, handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    // get verbose level
    bool is_verbose = verbose(argc, argv);

    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(is_verbose);

    // the usb connector
    shared_ptr<SerialIOInterface> sio =
            (shared_ptr<SerialIOInterface>) new SerialIOImpl(sb, is_verbose);

    // the usb handler
    shared_ptr<SerialHandler> sh = (shared_ptr<SerialHandler>) new SerialHandler();
    sh->set_usb_connector(sio);
    sh->set_verbose(is_verbose);

    // thread for the handler
    thread uhthread(&SerialHandler::run, sh);
    uhthread.detach();
    //sh->stop();

    proxy::Proxy proxy(argc, argv, sb);
    proxy.runModule();

    return 0;
}

/*
static void handler(int signum)
{
    exit(signum);
}
*/

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

