#ifndef USBCONNECTOR_H
#define USBCONNECTOR_H
// included dependencies
// ==================================================
#include <libusb-1.0/libusb.h>

// defined addresses and identifiers
// ==================================================
#define USB_VENDOR_ID	    0x0483      
#define USB_PRODUCT_ID	    0x5740      
#define USB_ENDPOINT_IN	    (LIBUSB_ENDPOINT_IN  | 1)   
#define USB_ENDPOINT_OUT    (LIBUSB_ENDPOINT_OUT | 2)
#define LEN_IN_BUFFER       1024


// define SerialConnector class
// ============================
class USBConnector
{
public:
    // constructor
    USBConnector();
    // destructor
    ~USBConnector();
    int connect(void);
    void read(void);
    void write(unsigned char *c);
    void disconnect(void);
private:
    int init_libusb(void);
    int open_device(void);
    int interface_taken(void);
    int claim_interface(void);
    unsigned char in_buffer[LEN_IN_BUFFER];
    libusb_device_handle *usb_dev;
    libusb_context *ctx;
    struct libusb_transfer *transfer_in;
    struct libusb_transfer *transfer_out;
};

#endif	// USBCONNECTOR_H

