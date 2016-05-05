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
#include "serial/USBHandler.h"

using namespace std;

// running mutex
std::mutex m_stop;

/*! constructor */
serial::handler::USBHandler::USBHandler() :
    verbose{false},
    running{false},
    uc{}
{
    cout << "creating usb handler... ";
    cout << "[OK]" << endl;
}


/*! destructor */
serial::handler::USBHandler::~USBHandler()
{
    cout << "destroying usb handler... ";
    cout << "[OK]" << endl;
}


/*! run */
void serial::handler::USBHandler::run()
{
    // call connect until true
    while (1) {
        if (uc->connect()) {
            //running = true;
            break;
        }
    }

    // main loop
    while (running) {
        m_stop.lock();
        // read from usb
        int res1 = uc->read();
        if (verbose) {
            cout << "result from read: " << res1 << endl;
        }
        // if not successful read, check if we
        // need to reconnect
        if (res1 != 0) {
            if (is_reconnect(res1)) reconnect();
        }
        // write it to usb
        int res2 = uc->write();
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
void serial::handler::USBHandler::stop()
{
    cout << "*** calling stop ***" << endl;
    m_stop.lock();
    running = false;
    m_stop.unlock();
}


/*! sets the usb connector for this handler */
void serial::handler::USBHandler::set_usb_connector(shared_ptr<serial::conninter::USBConnector> p_uc)
{
    uc = p_uc;
}


/*! sets verbose */
void serial::handler::USBHandler::set_verbose(bool a_ver)
{
    verbose = a_ver;
}


bool serial::handler::USBHandler::get_running()
{
    return running;
}


/*! reconnects the usb */
void serial::handler::USBHandler::reconnect()
{
    cout << "reconnecting..." << endl; 
    uc->disconnect();
    while (1) {       
        if (uc->connect()) break;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


/*! returns true if reconnection needed, false otherwise*/
bool serial::handler::USBHandler::is_reconnect(int error_code)
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
