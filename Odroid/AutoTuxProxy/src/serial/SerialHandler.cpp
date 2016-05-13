/**
 * Implementation of the SerialHandler.h. Responsible for managing the thread
 * to read and write from/to the serial io.
 *
 * @author Ivo
 */


#include <thread>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <serial/SerialIOImpl.h>
#include "serial/SerialHandler.h"

using namespace std;


/* constructor */
serial::SerialHandler::SerialHandler() :
    verbose{false},
    running{true},
    pserio{},
    pserbuf{}
{
    cout << "creating serial handler... ";
    cout << "[OK]" << endl;
}


/* destructor */
serial::SerialHandler::~SerialHandler()
{
    cout << "destroying serial handler... ";
    cout << "[OK]" << endl;
}


/* run */
void serial::SerialHandler::run()
{
    // call connect until true
    while (!pserio->connect()) {
        this_thread::sleep_for(chrono::seconds(2));
    }

    // main loop
    while (running) {
        // for measuring time for operations and adjust the frequency
        chrono::time_point<chrono::system_clock> start, end;
        // get the start time
        start = chrono::system_clock::now();
        // read
        int res1 = readOp();
        // if not successful read, check if we need to reconnect
        if (res1 != 0) {
            if (serial::is_reconnect(res1))
                reconnect();
        }
        // write
        int res2 = writeOp();
        // if not successful write, check if we need to reconnect
        if (res2 != 0) {
            if (serial::is_reconnect(res2))
                reconnect();
        }
        // get the end time
        end = chrono::system_clock::now();
        // get the elapsed time
        chrono::duration<double> duration = end - start;
        auto millisec = duration.count();
        if (millisec < 50) {
            // adjust the sleep time accordingly to keep the
            // required frequency
            int sleep = 50 - (int) millisec;
            this_thread::sleep_for(chrono::milliseconds(sleep));
        }
    }
}


/* read operation */
int serial::SerialHandler::readOp()
{
    // actual bytes read from the serial
    int read_bytes;
    // the char array where data will be stored
    unsigned char data[READ_LEN];
    // read from the serial io
    int res = pserio->read(data, &read_bytes);
    // print some info if verbose mode
    if (verbose) {
        cout << "result from read: " << res << endl;
        cout << "bytes read: " << read_bytes << endl;
    }
    // if success
    if (res == 0) {
        // the vector holding the data from the read
        vector<unsigned char> vec(data, data + read_bytes);
        // append to the receive buffer
        pserbuf->appendReceiveBuffer(vec);
    }
    // return the result, used to check if reconnection needed
    return res;
}


/* write operation */
int serial::SerialHandler::writeOp()
{
    // get the data to write to the serial
    vector<unsigned char> vec = pserbuf->readSendBuffer();
    // send it for write to serial io
    int res = pserio->write(vec);
    // print info if verbose
    if (verbose) {
        cout << "result from write: " << res << endl;
    }
    // return the result, used to check if reconnection needed
    return res;
}


/* stops the handler */
void serial::SerialHandler::stop()
{
    cout << "*** calling stop ***" << endl;
    running = false;
}


/* sets the serial io for this handler */
void serial::SerialHandler::set_serialio(
        shared_ptr<serial::SerialIOInterface> p)
{
    pserio = p;
}


/*! sets the serial buffer for this handler */
void serial::SerialHandler::set_buffer(
        shared_ptr<serial::SerialBuffer> p)
{
    pserbuf = p;
}


/* sets verbose */
void serial::SerialHandler::set_verbose(bool a_ver)
{
    verbose = a_ver;
}


/* reconnects */
bool serial::SerialHandler::reconnect()
{
    cout << "reconnecting..." << endl;
    pserio->disconnect();
    while (1) {       
        if (pserio->connect()) {
            return true;
        }
        this_thread::sleep_for(chrono::seconds(2));
    }
}


/* returns true if reconnection needed, false otherwise*/
bool serial::is_reconnect(int error_code)
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
