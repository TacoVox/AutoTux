/*!
 * USBHandler header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_USBHANDLER_H
#define AUTOTUXPROXY_USBHANDLER_H

#include <atomic>
#include "serial/BufferWrapper.h"
#include "serial/USBConnector.h"

namespace serial
{
    namespace handler
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
            void set_usb_connector(std::shared_ptr<serial::conninter::USBConnector>);
            //void set_usb_connector(serial::conninter::USBConnector*);
            /*! sets verbose */
            void set_verbose(bool);
            /*! returns running state */
            bool get_running();
        private:
            /*! reconnects the usb */
            void reconnect();
            /*! returns true if reconnection needed, false otherwise*/
            bool is_reconnect(int);
        private:
            /*! is it verbose mode */
            bool verbose;
            /*! loop control */
            volatile bool running;
            /*! the usb connector */
            std::shared_ptr<serial::conninter::USBConnector> uc;
            //serial::conninter::USBConnector *uc;
        };
    } // namespace handler
} // namespace serial

#endif	// AUTOTUXPROXY_USBHANDLER_H
