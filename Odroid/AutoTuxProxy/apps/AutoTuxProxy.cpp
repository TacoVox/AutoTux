//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <signal.h>
#include "camera/CameraProxy.h"
#include "serial/BufferWrapper.h"
#include "proxy/Proxy.h"
#include "serial/USBHandler.h"

using namespace std;
using namespace serial;
using namespace usb_handler;
using namespace proxy::camera;

void exit_handler(int);

int main(int argc, char **argv) {

    signal(SIGINT, exit_handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    shared_ptr<BufferWrapper> bw = (shared_ptr<BufferWrapper>)new BufferWrapper();

    proxy::Proxy proxy(argc, argv, bw);
    proxy.runModule();

    shared_ptr<USBHandler> uc = (shared_ptr<USBHandler>) new USBHandler(bw);
    thread ucthread(&USBHandler::run, uc);

    //Waiting for the thread to terminate
    ucthread.join();
    cout << "SerialHandler stopped" << endl;

    return 0;
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}
