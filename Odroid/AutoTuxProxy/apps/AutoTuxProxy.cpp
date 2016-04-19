//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <signal.h>
#include "serial/BufferWrapper.h"
#include "proxy/Proxy.h"
#include "serial/USBHandler.h"

using namespace std;
using namespace serial;
using namespace usb_handler;
using namespace proxy::camera;

void exit_handler(int);

int32_t main(int32_t argc, char **argv) {

    signal(SIGINT, exit_handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    shared_ptr<BufferWrapper> bw = (shared_ptr<BufferWrapper>)new BufferWrapper();

    shared_ptr<USBHandler> uc = (shared_ptr<USBHandler>) new USBHandler(bw);
    thread ucthread(&USBHandler::run, uc);
    ucthread.detach();
	
	cout << "Detatched USB thread"  << endl; 
    proxy::Proxy proxy(argc, argv, bw);
    proxy.runModule();

    return 0;
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}
