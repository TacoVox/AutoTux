/**
 * Serial io implementation header. Declares the functions and member variables.
 *
 * @author Ivo
 */

#ifndef AUTOTUXPROXY_SERIALIOIMPL_H
#define AUTOTUXPROXY_SERIALIOIMPL_H


#include <libusb-1.0/libusb.h>
#include "serial/SerialIOInterface.h"
#include "serial/SerialBuffer.h"

// STM vendor and product id, used for connecting to the USB
#define USB_VENDOR_ID	    0x0483
#define USB_PRODUCT_ID	    0x5740
// endpoints for reading and writing
#define USB_ENDPOINT_IN	    0x81
#define USB_ENDPOINT_OUT    0x01
// buffer size when reading from usb stream
#define READ_LEN            128
// error code on empty data to write
#define EMPTY_DATA          -13


namespace serial
{
    class SerialIOImpl : public serial::SerialIOInterface
    {
    public:
        /**
         * Default constructor.
         */
        SerialIOImpl();
        /**
         * Destructor.
         */
        ~SerialIOImpl();
        /**
         * Copy constructor.
         */
        SerialIOImpl(const SerialIOImpl&);
        /**
         * Copy constructor.
         */
        SerialIOImpl & operator=(const SerialIOImpl&);
        /**
         * Connects to the usb.
         *
         * @return  True on success, false otherwise.
         */
        bool connect() override;
        /**
         * Disconnects and closes the usb stream.
         *
         * @return  True on success, false otherwise.
         */
        bool disconnect() override;
        /**
         * Reads from the usb stream. Takes a pointer to the char array
         * where data will stored and an int pointer for the actual bytes
         * read from the stream.
         *
         * @param data  A pointer to the char array where to store data.
         * @param transferred   An int pointer for the actual bytes read.
         */
        int read(unsigned char *data, int *transferred) override;
        /**
         * Writes to the usb stream. Takes a vector holding the data to be
         * written.
         *
         * @param v     The vector holding the data to write.
         */
        int write(std::vector<unsigned char> v) override;
    private:
        /**
         * Gets a list of the devices and opens the one we need.
         *
         * @return  True on success, false otherwise.
         */
        bool open_device();
        /**
         * Claims the interface of the USB for I/O operations.
         *
         * @return  True on success, false otherwise.
         */
        bool claim_interface();
    private:
        /* libusb context */
        struct libusb_context *ctx;
        /* libusb device handle */
        struct libusb_device_handle *usb_dev;
    };
 } // namespace serial

#endif // AUTOTUXPROXY_SERIALIOIMPL_H
