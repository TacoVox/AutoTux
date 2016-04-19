
#include "serial/USBHandler.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>

using namespace std;


usb_handler::USBHandler::USBHandler(std::shared_ptr<usb_connector::USBConnector> c) :
    running{true},
    uc{c}
{

}


usb_handler::USBHandler::~USBHandler()
{
    uc->~USBConnector();
}


void usb_handler::USBHandler::run()
{
    // connect and set the buffer wrapper
    uc->connect();

    // main loop
    while (running) {
        // read from usb
        int res1 = uc->read();
        cout << "result from read: " << res1 << endl;
        if (res1 != 0) {
            if (is_reconnect(res1)) reconnect();
        }
        // write it to usb
        int res2 = uc->write();
        cout << "result from write: " << res2 << endl;
        if (res2 != 0) {
            if (is_reconnect(res2)) reconnect();
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


void usb_handler::USBHandler::stop()
{
    running = false;
}


void usb_handler::USBHandler::set_usb_connector(std::shared_ptr<usb_connector::USBConnector> c)
{
    uc = c;
}


void usb_handler::USBHandler::reconnect()
{
    cout << "reconnecting..." << endl; 
    uc->disconnect();
    while (1) {       
        if (uc->connect()) break;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}


bool usb_handler::USBHandler::is_reconnect(int error_code)
{
    switch (error_code) {
    case LIBUSB_ERROR_IO:
    case LIBUSB_ERROR_NOT_FOUND:
    case LIBUSB_ERROR_NO_DEVICE:
    case LIBUSB_ERROR_ACCESS:
    case LIBUSB_ERROR_PIPE:
    case LIBUSB_ERROR_NOT_SUPPORTED:
    case LIBUSB_ERROR_OTHER:
        return true;
        //break;
    }
    return false;
}
