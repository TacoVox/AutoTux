
#include "serial/USBHandler.h"
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;


usb_handler::USBHandler::USBHandler(std::shared_ptr<serial::BufferWrapper> p) :
    bw{p},
    uc()
{

}


usb_handler::USBHandler::~USBHandler()
{
    uc.~USBConnector();
}


void usb_handler::USBHandler::run()
{
    // connect and set the buffer wrapper
    uc.connect();
    uc.set_buffer_wrapper(bw);

    // main loop
    while (1) {
        // read from usb
        int res1 = uc.read();
        cout << "result from read: " << res1 << endl;
        if (res1 != 0) {
            if (res1 != -7)
                reconnect();
        }
        // write to usb
        int res2 = uc.write();
        cout << "result from write: " << res2 << endl;
        if (res2 != 0) {
            if (res2 != -13)
                reconnect();
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}


void usb_handler::USBHandler::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> p)
{
    bw = p;
}


void usb_handler::USBHandler::reconnect()
{
    cout << "reconnecting..." << endl; 
    while (1) {
        uc.disconnect();
        if (uc.connect()) break;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}
