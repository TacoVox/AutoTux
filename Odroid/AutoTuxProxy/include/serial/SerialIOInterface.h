/**
 * Serial io interface.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_SERIALIOINTERFACE_H
#define AUTOTUXPROXY_SERIALIOINTERFACE_H


#include <vector>

namespace serial
{
    class SerialIOInterface
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~SerialIOInterface();
        /**
         * Connects to the usb.
         *
         * @return  True on success, false otherwise.
         */
        virtual bool connect() = 0;
        /**
         * Disconnects and closes the usb stream.
         *
         * @return  True on success, false otherwise.
         */
        virtual bool disconnect() = 0;
        /**
         * Reads from the usb stream. Takes a pointer to the char array
         * where data will stored and an int pointer for the actual bytes
         * read from the stream.
         *
         * @param data  A pointer to the char array where to store data.
         * @param transferred   An int pointer for the actual bytes read.
         */
        virtual int read(unsigned char *data, int *transferred) = 0;
        /**
         * Writes to the usb stream. Takes a vector holding the data to be
         * written.
         *
         * @param v     The vector holding the data to write.
         */
        virtual int write(std::vector<unsigned char> v) = 0;
    };
} // namespace serial

#endif	// AUTOTUXPROXY_SERIALIOINTERFACE_H

