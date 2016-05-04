/*!
 * USBConnector header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef USBCONNECTOR_H
#define USBCONNECTOR_H
// include
// ==================================================
#include <memory>
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
#define READ_LEN            128
// error code on empty data to write
#define EMPTY_DATA          -13

// USBConnector class
// ============================
namespace serial
{
    class USBConnector
    {
    public:
        /*! constructor */
        USBConnector();
        /*! destructor */
        ~USBConnector();
        /*! copy constructor */
        USBConnector(const USBConnector&);
        /*! copy constructor */
        USBConnector & operator=(const USBConnector&);
        /*! connects to usb */
        bool connect();
        /*! disconnects and closes the usb stream */
        bool disconnect();
        /*! reads from the usb stream */
        int read();
        /*! writes to the usb stream */
        int write();
        /*! sets the buffer wrapper for this connector */
        void set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper>);
        /*! sets verbose */
        void set_verbose(bool);
    private:
        /*! initializes libusb */
        bool init_libusb();
        /*! gets a list of the devices and opens the one we need */
        bool open_device();
        /*! claims the interface of the USB for I/O operations */
        bool claim_interface();
    private:
        /*! is it verbose mode */
        bool verbose;
        /*! the buffer wrapper */
        std::shared_ptr<serial::BufferWrapper> bw;
        /*! libusb context */
        struct libusb_context *ctx;
        /*! libusb device handle */
        struct libusb_device_handle *usb_dev;
    };
} // namespace

#endif	// USBCONNECTOR_H

