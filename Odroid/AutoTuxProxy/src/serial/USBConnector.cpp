
#include <iostream>
#include <string>
#include "serial/USBConnector.h"
#include <thread>
#include <chrono>

using namespace std;
void callback_in(struct libusb_transfer *);
void callback_out(struct libusb_transfer *);

/* constructor */
usb_connector::USBConnector::USBConnector()
{
    cout << "creating a usb connector object... ";
    cout << "[OK]" << endl;
}


/* destructor */
usb_connector::USBConnector::~USBConnector()
{
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx);
}


/* initializes libusb */
int usb_connector::USBConnector::init_libusb(void)
{
    cout << "initializing libusb... ";
    int res = libusb_init(&ctx);
    if (res < 0) {
        cout << "[FAIL] error code: " << res << endl;
        return res;
    }
    cout << "[OK]" << endl;
    return 0;
}


void usb_connector::USBConnector::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> p)
{
    bw = p;
}


/* gets a list of the devices and opens the one we need */
int usb_connector::USBConnector::open_device(void)
{
    struct libusb_device **devs;
    ssize_t dev_count = libusb_get_device_list(ctx, &devs);
    cout << "finding devices... ";
    if (dev_count < 0) {
        cout << "[FAIL] error code: " << dev_count << endl;
        return dev_count;
    }
    cout << "[OK] " << endl;
    cout << "opening device... ";
    int open;
    for (ssize_t i = 0; i < dev_count; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout << "[FAIL] dev_desc" << endl;
            return r;
        }
        // if the device matches, open it
        if (desc.idVendor == USB_VENDOR_ID && desc.idProduct == USB_PRODUCT_ID) {
            open = libusb_open(devs[i], &usb_dev);
            break;
        }
    }
    if (open != 0) {
        cout << "[FAIL] error code: " << open << endl;
        return open;
    }
    cout << "[OK]" << endl;
    // free device list after we are done with it
    libusb_free_device_list(devs, 1);
    return 0;
}


/* checks if the interface of the USB is taken and frees it if so */
int usb_connector::USBConnector::interface_taken(void)
{
    int d = libusb_kernel_driver_active(usb_dev, 1);
    if (d == 1) {
        cout << "freeing interface... ";
        libusb_detach_kernel_driver(usb_dev, 1);
        cout << "[OK]" << endl;
    }
    else if (d < 0) {
        cout << "[FAIL] error code: " << d << endl;
        return d;
    }
    return 0;
}


/* claims the interface of the USB for I/O operations */
int usb_connector::USBConnector::claim_interface(void)
{
    cout << "claiming interface... ";
    int r = libusb_claim_interface(usb_dev, 1);
    if (r == 0) {
        cout << "[OK]" << endl;
    }
    else {
        cout << "[FAIL] error code: " << r << endl;
        return r;
    }
    return 0;
}


/* releases the taken usb interface and attaches the kernel driver */
void usb_connector::USBConnector::release_interface()
{
    cout << "releasing interface... ";
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    cout << "[OK]" << endl;
}


/* connects and opens stream to usb */
int usb_connector::USBConnector::connect(void)
{
    cout << "usb connecting..." << endl;
    if (init_libusb() == 0) {
        if (open_device() == 0)
            if (interface_taken() == 0)
                if (claim_interface() == 0) {
                    transfer_in  = libusb_alloc_transfer(0);
                    transfer_out = libusb_alloc_transfer(0);
                    cout << "[OK]" << endl;
                    return 0;
                }
    }
    cout << "[FAIL]" << endl;
    return -1;
}


/* reads from the usb stream */
void usb_connector::USBConnector::read(void)
{
    cout << "reading from usb stream..." << endl; 
    libusb_fill_bulk_transfer( transfer_in, usb_dev, USB_ENDPOINT_IN,
        in_buffer,  LEN_IN_BUFFER, callback_in, this, 0);
    libusb_submit_transfer(transfer_in);
    while (libusb_handle_events_completed(ctx, NULL) != LIBUSB_SUCCESS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


/* writes to the usb stream */
void usb_connector::USBConnector::write(string out)
{
    int len = out.length();
    if (len <= 0) return;
    char *pkt = new char[len + 1];
    out.copy(pkt, sizeof(pkt));
    cout << "writing to usb stream..." << endl;
    libusb_fill_bulk_transfer( transfer_out, usb_dev, USB_ENDPOINT_OUT,
        (unsigned char *)pkt,  len, callback_in, this, 0);
    libusb_submit_transfer(transfer_in);
    while (libusb_handle_events_completed(ctx, NULL) != LIBUSB_SUCCESS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    delete [] pkt;
}


/* disconnects and closes the usb stream*/
void usb_connector::USBConnector::disconnect(void)
{
    cout << "disconnecting from usb stream... ";
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx); 
    cout << "[OK]" << endl;
}


/* handles the callback when reading from the usb stream */
void usb_connector::USBConnector::handle_cb_in(unsigned char *transfer, int len)
{
    bw->appendReceiveBuffer(transfer, len);
}


/* handles the callback when writing to the usb stream */
void usb_connector::USBConnector::handle_cb_out(int status)
{
    cout << "transfer out status: " << status << endl;
}


/* callback when writing to the usb stream */
void callback_out(struct libusb_transfer *transfer)
{
    usb_connector::USBConnector *connector =
            reinterpret_cast<usb_connector::USBConnector*>(transfer->user_data);
    int status = transfer->status;
    connector->handle_cb_out(status);
}


/* callback when reading from the usb stream */
void callback_in(struct libusb_transfer *transfer)
{
    usb_connector::USBConnector *connector =
            reinterpret_cast<usb_connector::USBConnector*>(transfer->user_data);
    cout << "transfer actual length: " << transfer->actual_length << endl;
    connector->handle_cb_in(transfer->buffer, transfer->actual_length);
}
