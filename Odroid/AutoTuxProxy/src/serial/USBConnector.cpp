
#include <iostream>
#include <string>
#include "serial/USBConnector.h"
#include "serial/BufferParser.h"
#include <thread>
#include <chrono>

using namespace std;
void callback_in(struct libusb_transfer *transfer);

/* constructor */
usb_connector::USBConnector::USBConnector()
{
    cout << "Creating a USB object!" << endl;
    bp = (unique_ptr<buf_parser::BufferParser>) (new buf_parser::BufferParser());
}


/* destructor */
usb_connector::USBConnector::~USBConnector()
{
    // free resources here
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx);
    //delete in_buffer;
}

/* initialize libusb */
int usb_connector::USBConnector::init_libusb(void)
{
    cout << "initializing libusb..." << endl;
    int res = libusb_init(&ctx);
    if (res < 0) {
        cout << "failed to initialize libusb" << endl;
        return -1;
    }
    cout << "OK" << endl;
    return 0;
}

/* get a list of the devices and open the one we need */
int usb_connector::USBConnector::open_device(void)
{
    cout << "opening device..." << endl;
    libusb_device **devs;
    ssize_t dev_count = libusb_get_device_list(ctx, &devs);
    cout << "num devices found: " << dev_count << endl;
    if (dev_count < 0) {
        cout << "device error" << endl;
        return dev_count;
    }
    int open;
    for (ssize_t i = 0; i < dev_count; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout << "failed to get device descriptor" << endl;
            return r;
        }
        cout << "device " << i << ": " << "vendor id - " << desc.idVendor;
        cout << " | product id - " << desc.idProduct << endl;
        // if the device matches, open it
        if (desc.idVendor == USB_VENDOR_ID && desc.idProduct == USB_PRODUCT_ID) {
            cout << "device match" << endl;
            open = libusb_open(devs[i], &usb_dev);
            break;
        }
    }
    if (open != 0) {
        cout << "device not found, error code: " << open << endl;
        return open;
    }
    cout << "OK" << endl;
    // free device list after we are done with it
    libusb_free_device_list(devs, 1);
    return 0;
}

/* check if the interface of the USB is taken and free if so */
int usb_connector::USBConnector::interface_taken(void)
{
    int d = libusb_kernel_driver_active(usb_dev, 1);
    if (d == 1) {
        cout << "interface taken" << endl;
        cout << "freeing interface..." << endl;
        libusb_detach_kernel_driver(usb_dev, 1);
        cout << "interface free" << endl;
    }
    else if (d < 0) {
        cout << "interface problem" << endl;
        cout << "error code: " << d << endl;
        return d;
    }
    return 0;
}

/* claim the interface of the USB for I/O operations */
int usb_connector::USBConnector::claim_interface(void)
{
    cout << "claiming interface..." << endl;
    int r = libusb_claim_interface(usb_dev, 1);
    if (r == 0) {
        cout << "OK" << endl;
    }
    else {
        cout << "interface claim failed" << endl;
        cout << "error code: " << r << endl;
        return r;
    }
    return 0;
}

void usb_connector::USBConnector::release_interface()
{
    cout << "releasing interface..." << endl;
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    cout << "OK" << endl;
}

/* connect and open stream to usb */
int usb_connector::USBConnector::connect(void)
{
    cout << "usb connecting..." << endl;
    if (init_libusb() == 0) {
        if (open_device() == 0)
            if (interface_taken() == 0)
                if (claim_interface() == 0) {
                    cout << "usb connected" << endl;
                    return 0;
                }
    }
    cout << "usb connection failed" << endl;
    return -1;
}

/* read from the usb stream */
void usb_connector::USBConnector::read(void)
{
    cout << "reading from usb stream..." << endl;
    transfer_in  = libusb_alloc_transfer(0);
    while (1) {
        libusb_fill_bulk_transfer( transfer_in, usb_dev, USB_ENDPOINT_IN,
            in_buffer,  LEN_IN_BUFFER, callback_in, this, 0);
        libusb_submit_transfer(transfer_in);
        while (libusb_handle_events_completed(ctx, NULL) != LIBUSB_SUCCESS) {
            cout << "waiting to complete" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    libusb_free_transfer(transfer_in);
}

/* write to the usb stream */
void usb_connector::USBConnector::write(unsigned char *c)
{
    cout << "Writing to usb stream..." << endl;
}

/* disconnect and close the usb stream*/
void usb_connector::USBConnector::disconnect(void)
{
    cout << "disconnecting from usb stream..." << endl;
    // free resources here
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx); 
    cout << "OK" << endl;
}

void usb_connector::USBConnector::handle_callback(string transfer) {
    cout << "transfer actual_length = " << transfer.length() << endl;
    cout << "transfer data: " << transfer << endl;

}

void callback_in(struct libusb_transfer *transfer)
{
    cout << "call to callback_in" << endl;
    //cout << "buffer: " << endl;
    //cout << transfer->buffer << endl;
    usb_connector::USBConnector *connector =
            reinterpret_cast<usb_connector::USBConnector*>(transfer->user_data);
    string tran((char *)transfer->buffer);
    connector->handle_callback(tran);
}
