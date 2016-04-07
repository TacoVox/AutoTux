#ifndef USBCONNECTOR_H
#define USBCONNECTOR_H
// include
// ==================================================
#include <memory>
#include <string>
#include <libusb-1.0/libusb.h>
#include "serial/BufferParser.h"

// define
// ==================================================
// STM
#define USB_VENDOR_ID	    0x0483
#define USB_PRODUCT_ID	    0x5740
// Arduino
//#define USB_VENDOR_ID	    0x2341
//#define USB_PRODUCT_ID	    0x0042
#define USB_ENDPOINT_IN	    (LIBUSB_ENDPOINT_IN  | 1)   
#define USB_ENDPOINT_OUT    (LIBUSB_ENDPOINT_OUT | 2)
#define LEN_IN_BUFFER       1024

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
        void write(unsigned char *);
        void disconnect(void);
        void handle_callback(std::string);
    private:
        int init_libusb(void);
        int open_device(void);
        int interface_taken(void);
        int claim_interface(void);      
        unsigned char *in_buffer;
        std::unique_ptr<buf_parser::BufferParser> bp;
        struct libusb_device_handle *usb_dev;
        struct libusb_context *ctx;
        struct libusb_transfer *transfer_in;
        struct libusb_transfer *transfer_out;
    };
}

#endif	// USBCONNECTOR_H

