//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <signal.h>
<<<<<<< HEAD
#include "camera/CameraProxy.h"
#include "serial/BufferWrapper.h"
#include "proxy/Proxy.h"
#include "serial/USBHandler.h"

using namespace std;
using namespace serial;
using namespace usb_handler;
using namespace proxy::camera;
=======
#include <proxy/Proxy.h>


using namespace std;
>>>>>>> e28127035b45501a400dd34412bf52772b0e19a1

void exit_handler(int);

int32_t main(int32_t argc, char **argv) {

    signal(SIGINT, exit_handler);

    cout << "Starting up AutoTuxProxy..." << endl;

<<<<<<< HEAD
    shared_ptr<BufferWrapper> bw = (shared_ptr<BufferWrapper>)new BufferWrapper();

    proxy::Proxy proxy(argc, argv, bw);
    proxy.runModule();

    shared_ptr<USBHandler> uc = (shared_ptr<USBHandler>) new USBHandler(bw);
    thread ucthread(&USBHandler::run, uc);

    //Waiting for the thread to terminate
    ucthread.join();
    cout << "SerialHandler stopped" << endl;

    return 0;
=======
    proxy::Proxy p(argc, argv);

    return p.runModule();
>>>>>>> e28127035b45501a400dd34412bf52772b0e19a1
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}
