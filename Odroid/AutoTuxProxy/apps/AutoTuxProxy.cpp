//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <csignal>
#include <serial/USBConnector_impl.h>
#include "serial/BufferWrapper.h"
#include "proxy/Proxy.h"
#include "serial/USBHandler.h"

using namespace std;
using namespace serial::interface;
using namespace serial::connector;
using namespace serial::handler;
using namespace serial::buffer;
using namespace proxy::camera;


//static void handler(int);
bool verbose(int, char**);

int32_t main(int32_t argc, char **argv) {

    //signal(SIGINT, handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    // get verbose level
    bool is_verbose = verbose(argc, argv);

    // the buffer wrapper
    shared_ptr<BufferWrapper> bw =
            (shared_ptr<BufferWrapper>) new BufferWrapper(is_verbose);

    // the usb connector
    shared_ptr<USBConnector> uc =
            (shared_ptr<USBConnector>) new USBConnector_impl(bw, is_verbose);

    // the usb handler
    shared_ptr<USBHandler> uh = (shared_ptr<USBHandler>) new USBHandler();
    uh->set_usb_connector(uc);
    uh->set_verbose(is_verbose);

    // thread for the handler
    thread uhthread(&USBHandler::run, uh);
    uhthread.detach();

    proxy::Proxy proxy(argc, argv, bw);
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

