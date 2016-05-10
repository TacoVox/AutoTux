/*!
 * USBHandler header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_SERIALHANDLER_H
#define AUTOTUXPROXY_SERIALHANDLER_H

#include "serial/SerialBuffer.h"
#include "serial/SerialIOInterface.h"

namespace serial
{
    class SerialHandler
    {
    public:
        /*! constructor */
        SerialHandler();
        /*! destructor */
        ~SerialHandler();
        /*! run function for the thread */
        void run();
        /*! stops the handler, sets the loop control variable to false */
        void stop();
        /*! sets the usb connector for this handler */
        void set_serialio(std::shared_ptr<serial::SerialIOInterface>);
        /*! sets the usb connector for this handler */
        void set_buffer(std::shared_ptr<serial::SerialBuffer>);
        /*! sets verbose */
        void set_verbose(bool);
        int readOp();
        int writeOp();
        /*! reconnects the usb */
        bool reconnect();
    private:
        /*! is it verbose mode */
        bool verbose;
        /*! loop control */
        volatile bool running;
        /*! the serial io */
        std::shared_ptr<serial::SerialIOInterface> pserio;
        /*! the serial buffer */
        std::shared_ptr<serial::SerialBuffer> pserbuf;
    };

    /*! returns true if reconnection needed, false otherwise*/
    bool is_reconnect(int);

} // namespace serial

#endif	// AUTOTUXPROXY_SERIALHANDLER_H
