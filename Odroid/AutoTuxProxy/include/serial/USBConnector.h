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
#include <serial/IUSBConnector.h>
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
namespace usb_connector
{
    class USBConnector : public IUSBConnector
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
        bool connect(void);
        /*! disconnects and closes the usb stream */
        bool disconnect(void);
        /*! reads from the usb stream */
        int read(void);
        /*! writes to the usb stream */
        int write(void);       
        /*! sets the buffer wrapper for this connector */
        void set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        /*! initializes libusb */
        bool init_libusb(void);
        /*! gets a list of the devices and opens the one we need */
        bool open_device(void);
        /*! claims the interface of the USB for I/O operations */
        bool claim_interface(void);
    private:
        /*! the buffer wrapper */
        std::shared_ptr<serial::BufferWrapper> bw;
        /*! libusb context */
        struct libusb_context *ctx;
        /*! libusb device handle */
        struct libusb_device_handle *usb_dev;
    };
} // namespace usb_connector

#endif	// USBCONNECTOR_H

