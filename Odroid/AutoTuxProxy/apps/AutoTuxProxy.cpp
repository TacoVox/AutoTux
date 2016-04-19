//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <csignal>
#include "serial/BufferWrapper.h"
#include "proxy/Proxy.h"
#include "serial/USBHandler.h"

using namespace std;
using namespace serial;
using namespace usb_handler;
using namespace usb_connector;
using namespace proxy::camera;

shared_ptr<USBHandler> uh;

static void handler(int);

int32_t main(int32_t argc, char **argv) {

    signal(SIGINT, handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    shared_ptr<BufferWrapper> bw = (shared_ptr<BufferWrapper>) new BufferWrapper();
    shared_ptr<USBConnector> uc = (shared_ptr<USBConnector>) new USBConnector();
    uh = (shared_ptr<USBHandler>) new USBHandler(uc);

    uc->set_buffer_wrapper(bw);

    thread ucthread(&USBHandler::run, uh);
    ucthread.detach();

    proxy::Proxy proxy(argc, argv, bw);
    proxy.runModule();

    return 0;
}


static void handler(int signum)
{
    cout << "caught signal: " << signum << endl;
    uh->stop();
    uh->~USBHandler();
    exit(signum);
}

