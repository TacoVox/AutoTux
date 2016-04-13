//
// Created by jonas on 4/1/16.
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

int main(int argc, char **argv) {

    cout << "Starting up AutoTuxProxy..." << endl;

    //Setup the serialhandler
    serialHandler = (shared_ptr<SerialHandler>)new SerialHandler(argc, argv);
    thread shthread(&SerialHandler::run, serialHandler);

    //Setup the cameraproxy
    cameraProxy= (shared_ptr<CameraProxy>)new CameraProxy(argc, argv);
    thread cpthread(&CameraProxy::runModule, cameraProxy);

    //Waiting for the thread to terminate
    shthread.join();
    cpthread.join();

    return 0;
}

