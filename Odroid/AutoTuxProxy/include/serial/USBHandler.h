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
        /*! run function for the thread */
        void run();
        /*! stops the handler, sets the loop control variable to false */
        void stop();
        /*! sets the usb connector for this handler */
        void set_usb_connector(std::shared_ptr<usb_connector::USBConnector>);
    private:
        /*! reconnects the usb */
        void reconnect();
        /*! returns true if reconnection needed, false otherwise*/
        bool is_reconnect(int);
    private:
        /*! loop control */
        bool running;
        /*! the usb connector */
        std::shared_ptr<usb_connector::USBConnector> uc;

    };
} // namespace usb_handler

#endif	// USBHANDLER_H
