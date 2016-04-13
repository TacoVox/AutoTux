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

void exit_handler(int);
shared_ptr<SerialHandler> serialHandler;
shared_ptr<CameraProxy> cameraProxy;

int main(int argc, char **argv) {

    //Structs that will describe the old action and the new action
    //associated to the SIGINT signal (Ctrl+c from keyboard).
    struct sigaction new_action, old_action;

    //Set the handler in the new_action struct
    new_action.sa_handler = exit_handler;
    //Set to empty the sa_mask. It means that no signal is blocked
    // while the handler run.
    sigemptyset(&new_action.sa_mask);
    //Block the SEGTERM signal.
    // It means that while the handler run, the SIGTERM signal is ignored
    sigaddset(&new_action.sa_mask, SIGTERM);
    //Remove any flag from sa_flag. See documentation for flags allowed
    new_action.sa_flags = 0;

    //Read the old signal associated to SIGINT (keyboard, see signal(7))
    sigaction(SIGINT, NULL, &old_action);

    //If the old handler wasn't SIG_IGN (it's a handler that just
    // "ignore" the signal)
    if (old_action.sa_handler != SIG_IGN)
    {
        //Replace the signal handler of SIGINT with the one described by new_action
        sigaction(SIGINT,&new_action,NULL);
    }

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

void exit_handler(int exitcode)
{
    cout << "cleaning and exiting: " << exitcode << endl;
    serialHandler->interrupt();
    serialHandler->~SerialHandler();
}
