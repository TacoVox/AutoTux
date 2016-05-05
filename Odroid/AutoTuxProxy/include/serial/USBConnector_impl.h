//
// Created by ivo on 5/5/16.
//

#ifndef AUTOTUXPROXY_USBCONNECTOR_IMPL_H
#define AUTOTUXPROXY_USBCONNECTOR_IMPL_H


#include <memory>
#include <libusb-1.0/libusb.h>
#include "serial/USBConnector.h"
#include "serial/BufferWrapper.h"

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


namespace serial
{
    namespace connector
    {
        class USBConnector_impl : public serial::conninter::USBConnector
        {
        public:
            /*! constructor */
            USBConnector_impl();
            /*! constructor */
            USBConnector_impl(std::shared_ptr<serial::buffer::BufferWrapper>, bool);
            /*! destructor */
            ~USBConnector_impl();
            /*! copy constructor */
            USBConnector_impl(const USBConnector_impl&);
            /*! copy constructor */
            USBConnector_impl & operator=(const USBConnector_impl&);
            /*! connects to usb */
            bool connect() override;
            /*! disconnects and closes the usb stream */
            bool disconnect() override;
            /*! reads from the usb stream */
            int read() override;
            /*! writes to the usb stream */
            int write() override;
            /*! sets the buffer wrapper for this connector */
            void set_buffer_wrapper(std::shared_ptr<serial::buffer::BufferWrapper>);
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
            std::shared_ptr<serial::buffer::BufferWrapper> bw;
            /*! libusb context */
            struct libusb_context *ctx;
            /*! libusb device handle */
            struct libusb_device_handle *usb_dev;
        };
    } // namespace connector
 } // namespace serial

#endif // AUTOTUXPROXY_USBCONNECTOR_IMPL_H
