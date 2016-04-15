//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <signal.h>
#include "serial/SerialHandler.h"
#include "camera/CameraProxy.h"


using namespace std;
using namespace serial;
using namespace proxy::camera;

shared_ptr<SerialHandler> serialHandler;
shared_ptr<CameraProxy> cameraProxy;
void exit_handler(int);

int main(int argc, char **argv) {

    signal(SIGINT, exit_handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    //Setup the SerialHandler
    serialHandler = (shared_ptr<SerialHandler>)new SerialHandler(argc, argv);
    thread shthread(&SerialHandler::runModule, serialHandler);

    //Setup the CameraProxy
    cameraProxy = (shared_ptr<CameraProxy>)new CameraProxy(argc, argv);
    thread cpthread(&CameraProxy::runModule, cameraProxy);

    //Waiting for the thread to terminate
    shthread.join();
    cout << "SerialHandler stopped" << endl;
    cpthread.join();
    cout << "CameraHandler stopped" << endl;

    return 0;
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}
