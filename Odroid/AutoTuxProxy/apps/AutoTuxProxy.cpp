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
void exit_handler(int);

int main(int argc, char **argv) {

    struct sigaction signal_handler;

    signal_handler.sa_handler = exit_handler;
    sigemptyset(&signal_handler.sa_mask);
    signal_handler.sa_flags = 0;
    sigaction(SIGINT, &signal_handler, NULL);

    cout << "Starting up AutoTuxProxy..." << endl;

    //Setup the serialhandler
    serialHandler = (shared_ptr<SerialHandler>)new SerialHandler(argc, argv);
    thread shthread(&SerialHandler::run, serialHandler);
    //shthread.detach();

    //Setup the cameraproxy
    cameraProxy= (shared_ptr<CameraProxy>)new CameraProxy(argc, argv);
    thread cpthread(&CameraProxy::runModule, cameraProxy);
    //cpthread.detach();

    //Waiting for the thread to terminate
    shthread.join();
    cout << "serial handler stopped" << endl;
    cpthread.join();
    cout << "camera handler stopped" << endl;

    return 0;
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}

