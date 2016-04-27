/*!
 * USBHandler header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef USBHANDLER_H
#define USBHANDLER_H

#include "serial/BufferWrapper.h"
#include "serial/USBConnector.h"

namespace usb_handler {
    class USBHandler
    {
    public:
        /*! constructor */
        USBHandler(std::shared_ptr<usb_connector::USBConnector>);
        /*! destructor */
        ~USBHandler();
        void run();
        void stop();
        void set_usb_connector(std::shared_ptr<usb_connector::USBConnector>);
    private:
        void reconnect();
        bool is_reconnect(int);
    private:
        bool running;
        std::shared_ptr<usb_connector::USBConnector> uc;

    };
} // namespace usb_handler

#endif	// USBHANDLER_H
