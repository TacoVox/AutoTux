/*!
 * Implementation of the USBHandler.h. Responsible for managing the thread
 * to read and write to/from the usb.
 *
 * @author Ivo
 */


#include <thread>
#include <mutex>
#include <iostream>
#include <cstring>
#include <libusb-1.0/libusb.h>
#include "serial/SerialHandler.h"

using namespace std;

// running mutex
std::mutex m_stop;

/*! constructor */
serial::SerialHandler::SerialHandler() :
    verbose{false},
    running{false},
    pserial{}
{
    cout << "creating usb handler... ";
    cout << "[OK]" << endl;
}


/*! destructor */
serial::SerialHandler::~SerialHandler()
{
    cout << "destroying usb handler... ";
    cout << "[OK]" << endl;
}


/*! run */
void serial::SerialHandler::run()
{
    // call connect until true
    while (1) {
        if (pserial->connect()) {
            //running = true;
            break;
        }
    }

    // main loop
    while (running) {
        m_stop.lock();
        // read from usb
        int res1 = pserial->read();
        if (verbose) {
            cout << "result from read: " << res1 << endl;
        }
        // if not successful read, check if we
        // need to reconnect
        if (res1 != 0) {
            if (is_reconnect(res1)) reconnect();
        }
        // write it to usb
        int res2 = pserial->write();
        if (verbose) {
            cout << "result from write: " << res2 << endl;
        }
        // if not successful write, check if we
        // need to reconnect
        if (res2 != 0) {
            if (is_reconnect(res2)) reconnect();
        }
        m_stop.unlock();
        // sleep for 67, approximation to keep the frequency (30)
        this_thread::sleep_for(chrono::milliseconds(67));
    }
}


/*! stops the handler, sets the loop control variable to false */
void serial::SerialHandler::stop()
{
    cout << "*** calling stop ***" << endl;
    m_stop.lock();
    running = false;
    m_stop.unlock();
}


/*! sets the usb connector for this handler */
void serial::SerialHandler::set_usb_connector(shared_ptr<serial::SerialIOInterface> p)
{
    pserial = p;
}


/*! sets verbose */
void serial::SerialHandler::set_verbose(bool a_ver)
{
    verbose = a_ver;
}


bool serial::SerialHandler::get_running()
{
    return running;
}


/*! reconnects the usb */
void serial::SerialHandler::reconnect()
{
    cout << "reconnecting..." << endl; 
    pserial->disconnect();
    while (1) {       
        if (pserial->connect()) break;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


/*! returns true if reconnection needed, false otherwise*/
bool serial::SerialHandler::is_reconnect(int error_code)
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
