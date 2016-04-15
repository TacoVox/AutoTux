
#include "serial/USBHandler.h"
#include <thread>
#include <chrono>

using namespace std;


usb_handler::USBHandler::USBHandler(std::shared_ptr<serial::BufferWrapper> p) :
    bw{p},
    uc()
{

}


usb_handler::USBHandler::~USBHandler()
{

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
        if (res1 != 0) {
            reconnect();
        }
        // write to usb
        int res2 = uc.write();
        if (res2 != 0) {
            if (res2 == -13)
                continue;
            else
                reconnect();
        }
        // sleep a little
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}


void usb_handler::USBHandler::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> p)
{
    bw = p;
}

void usb_handler::USBHandler::reconnect()
{
    uc.disconnect();
    while (1) {
        if (uc.connect()) break;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}
