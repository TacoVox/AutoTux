/*!
 * Implementation of the USBHandler.h. Responsible for managing the thread
 * to read and write to/from the usb.
 *
 * @author Ivo
 */


#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include "serial/USBHandler.h"

using namespace std;


/*! constructor */
usb_handler::USBHandler::USBHandler(std::shared_ptr<usb_connector::USBConnector> ptr) :
    running{true},
    uc{ptr}
{
    cout << "creating usb handler... ";
    cout << "[OK]" << endl;
}


/*! destructor */
usb_handler::USBHandler::~USBHandler()
{
    cout << "destroying usb handler... ";
    uc->~USBConnector();
    cout << "[OK]" << endl;
}


/*! run function for the thread */
void usb_handler::USBHandler::run()
{
    // connect and set the buffer wrapper
    while (uc->connect() == false)
        ;

    // main loop
    while (running) {
        // read from usb
        int res1 = uc->read();
        cout << "result from read: " << res1 << endl;
        // if not successful read, check if we
        // need to reconnect
        if (res1 != 0) {
            if (is_reconnect(res1)) reconnect();
        }
        // write it to usb
        int res2 = uc->write();
        cout << "result from write: " << res2 << endl;
        // if not successful write, check if we
        // need to reconnect
        if (res2 != 0) {
            if (is_reconnect(res2)) reconnect();
        }
        // sleep for 67, approximation to keep the frequency (30)
        this_thread::sleep_for(chrono::milliseconds(67));
    }
}


/*! stops the handler, sets the loop control variable to false */
void usb_handler::USBHandler::stop()
{
    running = false;
}


/*! sets the usb connector for this handler */
void usb_handler::USBHandler::set_usb_connector(std::shared_ptr<usb_connector::USBConnector> ptr)
{
    cout << "setting usb connector to usb handler" << endl;
    uc = ptr;
}


/*! reconnects the usb */
void usb_handler::USBHandler::reconnect()
{
    cout << "reconnecting..." << endl; 
    uc->disconnect();
    while (1) {       
        if (uc->connect()) break;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


/*! returns true if reconnection needed, false otherwise*/
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
    default:
        return false;
    }
}
