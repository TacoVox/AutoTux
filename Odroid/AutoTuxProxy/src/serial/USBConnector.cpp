
#include <iostream>
#include "USBConnector.h"
#include <libusb-1.0/libusb.h>

using namespace std;
void cb_in(struct libusb_transfer *transfer);

/* constructor */
USBConnector::USBConnector() {
    cout << "Creating a USB object!" << endl;
}

/* copy constructor */
USBConnector::USBConnector(const USBConnector &sc) {

}

/* assignment */
USBConnector & USBConnector::operator=(const USBConnector &sc) {

}

/* destructor */
USBConnector::~USBConnector() {
    free(transfer_in);
    free(transfer_out);
    free(usb_dev);
    free(ctx);
}

/* connect and open stream to usb */
void USBConnector::connect() {
    cout << "usb connecting..." << endl;
    int res = 1;
    ssize_t dev_count;
    // initialize libusb
    cout << "initializing libusb..." << endl;
    res = libusb_init(&ctx);
    libusb_set_debug(ctx, 3);
    if (res < 0) {
        cout << "failed to initialize libusb" << endl;
        return;
    }
    cout << "OK" << endl;
    // open the device
    cout << "opening device..." << endl;
    dev_count = libusb_get_device_list(ctx, &devs);
    if (dev_count < 0) {
        cout << "device error" << endl;
    }
    ssize_t i;
    int u;
    for (i = 0; i < dev_count; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout << "failed to get device descriptor" << endl;
            return;
        }
        // if the device matches, open it
        if (desc.idVendor == USB_VENDOR_ID && desc.idProduct == USB_PRODUCT_ID) {
            cout << "found" << endl;
            cout << "vendor id = " << desc.idVendor << " | ";
            cout << "product id = " << desc.idProduct << endl;
            u = libusb_open(devs[i], &usb_dev);
            break;
        }
    }
    if (u) {
        cout << "device not found" << endl;
        return;
    }
    cout << "OK" << endl;
    
    int d; 
    d = libusb_kernel_driver_active(usb_dev, 1);
    if (d == 1) {
        cout << "kernel driver active" << endl;
        libusb_detach_kernel_driver(usb_dev, 1);
    }
    else if (d == 0){
        cout << "kernel driver NOT active" << endl;
    }
    else {
        cout << "something else with kernel" << endl;
        return;
    }
    // claim the interface
    cout << "claiming interface..." << endl;
    int r = libusb_claim_interface(usb_dev, 1);
    if (r == 0) {
        cout << "OK" << endl;
    }
    else {
        cout << "claim interface failed" << endl;
        if (r == LIBUSB_ERROR_NOT_FOUND) {
            cout << "interface not claimed" << endl;
        }
        else if (r == LIBUSB_ERROR_NO_DEVICE) {
            cout << "device disconnected" << endl;
        }
        else {
            cout << "other error code: " << r << endl;
        }
    }
    // free device list after we are done with it
    libusb_free_device_list(devs, 1);
}

/* read from the usb stream */
void USBConnector::read() {
    cout << "Reading from usb stream..." << endl;
    
    transfer_in  = libusb_alloc_transfer(0);
    libusb_fill_bulk_transfer( transfer_in, usb_dev, USB_ENDPOINT_IN,
        in_buffer,  LEN_IN_BUFFER, cb_in, NULL, 0);
    libusb_submit_transfer(transfer_in);
    
    while(1) {
        if(libusb_handle_events(NULL) != LIBUSB_SUCCESS) break;
    }
    
}

/* write to the usb stream */
void USBConnector::write(unsigned char c) {
    cout << "Writing to usb stream..." << endl;
}

/* disconnect and close the usb stream*/
void USBConnector::disconnect() {
    cout << "Disconnecting from usb stream..." << endl;
    // free resources here
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 0);
    libusb_close(usb_dev);
    libusb_exit(ctx); 
}

void cb_in(struct libusb_transfer *transfer) {
    if ( NULL == transfer){
        cout << "No libusb_transfer..." << endl;
    }
    else {
        cout << "libusb_transfer structure: " << endl;
        cout << "flags = " << transfer->flags << endl;
        cout << "endpoint = " << transfer->endpoint << endl;
        cout << "type = " << transfer->type << endl;
        cout << "timeout = " << transfer->timeout << endl;
        cout << "length = " << transfer->length << endl;
        cout << "actual_length = " << transfer->actual_length << endl;
        cout << "buffer = " << transfer->buffer << endl;
    }
    return;
}

