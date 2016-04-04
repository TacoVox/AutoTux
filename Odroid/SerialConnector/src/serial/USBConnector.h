#ifndef USBCONNECTOR_H
#define USBCONNECTOR_H
// included dependencies
// ============================
#include <libusb-1.0/libusb.h>
/* USB vendor ID used by the device 0x0483 is STMs ID */
#define USB_VENDOR_ID	    0x0483      
/* USB product ID used by the device */
#define USB_PRODUCT_ID	    0x5740      
/* endpoint addresses */
#define USB_ENDPOINT_IN	    (LIBUSB_ENDPOINT_IN  | 1)   
#define USB_ENDPOINT_OUT    (LIBUSB_ENDPOINT_OUT | 2)
#define LEN_IN_BUFFER 1024*8


// define SerialConnector class
// ============================
class USBConnector
{
public:
    // constructor
    USBConnector();
    // copy constructor
    USBConnector(const USBConnector &sc);
    // assignment
    USBConnector & operator=(const USBConnector &sc);
    // destructor
    ~USBConnector();
    void connect();
    void read();
    void write(unsigned char c);
    void disconnect();
    //void cb_in(struct libusb_transfer *transfer);
private:
    unsigned char in_buffer[LEN_IN_BUFFER];
    libusb_device_handle *usb_dev;
    libusb_context *ctx;
    libusb_device **devs;
    struct libusb_transfer *transfer_in;
    struct libusb_transfer *transfer_out;
};

#endif	// USBCONNECTOR_H

