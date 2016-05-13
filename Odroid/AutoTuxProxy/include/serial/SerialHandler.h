/**
 * Serial handler header. Declares the functions and member variables.
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
        /**
         * Constructor.
         */
        SerialHandler();
        /**
         * Destructor.
         */
        ~SerialHandler();
        /**
         * Run function for the thread.
         */
        void run();
        /**
         * Stops the handler.
         */
        void stop();
        /**
         * Sets the serial io for this handler. Takes a shared pointer to the
         * serial io as parameter.
         *
         * @param p     A shared pointer to the serial io.
         */
        void set_serialio(std::shared_ptr<serial::SerialIOInterface> p);
        /**
         * Sets the serial buffer for this handler. Takes a shared pointer
         * to the serial buffer as parameter.
         *
         * @param p     A shared pointer to the serial buffer.
         */
        void set_buffer(std::shared_ptr<serial::SerialBuffer> p);
        /**
         * Sets verbose mode for the handler. Takes a boolean as parameter.
         *
         * @param verbose   True for verbose, false otherwise.
         */
        void set_verbose(bool verbose);
        /**
         * Read operation.
         *
         * @return  The status code from the read operation.
         */
        int readOp();
        /**
         * Write operation.
         *
         * @return  The status code from the write operation.
         */
        int writeOp();
        /**
         * Reconnects the serial io in case the connection is lost.
         *
         * @return  True on success.
         */
        bool reconnect();
    private:
        /* verbose mode */
        bool verbose;
        /* loop control */
        volatile bool running;
        /* the serial io */
        std::shared_ptr<serial::SerialIOInterface> pserio;
        /* the serial buffer */
        std::shared_ptr<serial::SerialBuffer> pserbuf;
    };

    /**
     * Helper function. Checks if reconnection is needed.
     *
     * @return  True if reconnection needed, false otherwise.
     */
    bool is_reconnect(int);

} // namespace serial

#endif	// AUTOTUXPROXY_SERIALHANDLER_H
