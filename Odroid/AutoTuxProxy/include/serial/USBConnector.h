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
#define USB_ENDPOINT_IN	    0x81
#define USB_ENDPOINT_OUT    0x01
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
        USBConnector(const USBConnector&);
        USBConnector & operator=(const USBConnector&);
        bool connect(void);
        void read(void);
        void write(void);
        void disconnect(void);
        void handle_cb_in(unsigned char *, int);
        void handle_cb_out(int);
        void set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        bool init_libusb(void);
        bool open_device(void);
        bool claim_interface(void);
        bool release_interface(void);
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

