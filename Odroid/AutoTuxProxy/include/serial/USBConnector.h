#ifndef USBCONNECTOR_H
#define USBCONNECTOR_H
// include
// ==================================================
#include <memory>
#include <string>
#include <libusb-1.0/libusb.h>
#include "serial/BufferWrapper.h"

// define
// ==================================================
// STM
#define USB_VENDOR_ID	    0x0483
#define USB_PRODUCT_ID	    0x5740
// endpoints for reading and writing
#define USB_ENDPOINT_IN	    (LIBUSB_ENDPOINT_IN  | 1)   
#define USB_ENDPOINT_OUT    (LIBUSB_ENDPOINT_OUT | 2)
// buffer size when reading from usb stream
#define LEN_IN_BUFFER       128

// USBConnector class
// ============================
namespace usb_connector
{
    class USBConnector
    {
    public:
        USBConnector();
        ~USBConnector();
        int connect(void);
        void read(void);
        void write(void);
        void disconnect(void);
        void handle_cb_in(unsigned char *, int);
        void handle_cb_out(int);
        void set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        int init_libusb(void);
        int open_device(void);
        int interface_taken(void);
        int claim_interface(void);
        void release_interface(void);
    private:
        std::shared_ptr<serial::BufferWrapper> bw;
        unsigned char in_buffer[LEN_IN_BUFFER];
        struct libusb_device_handle *usb_dev;
        struct libusb_context *ctx;
        struct libusb_transfer *transfer_in;
        struct libusb_transfer *transfer_out;
    };
} // namespace usb_connector

#endif	// USBCONNECTOR_H

