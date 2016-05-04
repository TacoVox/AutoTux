/*!
 * USBHandler header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef USBHANDLER_H
#define USBHANDLER_H

#include "serial/BufferWrapper.h"
#include "serial/USBConnector.h"

namespace serial
{
    class USBHandler
    {
    public:
        /*! constructor */
        USBHandler();
        /*! destructor */
        ~USBHandler();
        /*! run function for the thread */
        void run();
        /*! stops the handler, sets the loop control variable to false */
        void stop();
        /*! sets the usb connector for this handler */
        //void set_usb_connector(std::shared_ptr<serial::USBConnector>);
        void set_usb_connector(serial::USBConnector&);
        /*! sets verbose */
        void set_verbose(bool);
    private:
        /*! reconnects the usb */
        void reconnect();
        /*! returns true if reconnection needed, false otherwise*/
        bool is_reconnect(int);
    private:
        /*! is it verbose mode */
        bool verbose;
        /*! loop control */
        bool running;
        /*! the usb connector */
        //std::shared_ptr<serial::USBConnector> uc;
        serial::USBConnector *uc;
    };
} // namespace

#endif	// USBHANDLER_H
