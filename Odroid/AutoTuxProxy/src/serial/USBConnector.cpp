
#include <iostream>
#include <cstdio>
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


/* copy constructor */
usb_connector::USBConnector::USBConnector(const usb_connector::USBConnector &usb)
{}


/* copy constructor */
usb_connector::USBConnector &
usb_connector::USBConnector::operator=(const usb_connector::USBConnector &usb)
{
    return *this;
}


/* destructor */
usb_connector::USBConnector::~USBConnector()
{
    cout << "destroying usb object... ";
    // release all resources here
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx);
    cout << "[OK]" << endl;
}


/* initializes libusb */
bool usb_connector::USBConnector::init_libusb(void)
{
    cout << "initializing libusb... ";
    int res = libusb_init(&ctx);
    if (res < 0) {
        cout << "[FAIL] error code: " << res << endl;
        return false;
    }
    cout << "[OK]" << endl;
    return true;
}


/* sets the buffer wrapper for this usb connector */
void usb_connector::USBConnector::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> p)
{
    bw = p;
}


/* gets a list of the devices and opens the one we need */
bool usb_connector::USBConnector::open_device(void)
{
    struct libusb_device **devs;
    // get available devices
    ssize_t dev_count = libusb_get_device_list(ctx, &devs);
    cout << "finding devices... ";
    if (dev_count < 0) {
        cout << "[FAIL] error code: " << dev_count << endl;
        return false;
    }
    cout << "[OK] " << endl;
    cout << "opening device... ";
    // go through the devices list searching for the one we need
    for (ssize_t i = 0; i < dev_count; i++) {
        libusb_device_descriptor desc;
        // for each device in the list, get its descriptor
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout << "[FAIL] dev_desc" << endl;
            return r;
        }
        // if the device matches, open it
        if (desc.idVendor == USB_VENDOR_ID && desc.idProduct == USB_PRODUCT_ID) {
            int open = libusb_open(devs[i], &usb_dev);
            if (open != 0) {
                cout << "[FAIL] error code: " << open << endl;
                return false;
            }
            break;
        }
    }

    cout << "[OK]" << endl;
    // free devices list after we are done with it
    libusb_free_device_list(devs, 1);
    return true;
}


/* claims the interface of the USB for I/O operations */
bool usb_connector::USBConnector::claim_interface(void)
{
    // check if the kernel is active on the device we want
    // to operate on
    int d = libusb_kernel_driver_active(usb_dev, 1);
    if (d == 1) {
        cout << "freeing interface... ";
        // detach kernel if active
        libusb_detach_kernel_driver(usb_dev, 1);
        cout << "[OK]" << endl;
    }
    else if (d < 0) {
        cout << "[FAIL] error code: " << d << endl;
        return false;
    }
    cout << "claiming interface... ";
    // claim the interface for the device, this will let us
    // send and receive data
    int r = libusb_claim_interface(usb_dev, 1);
    if (r == 0) {
        cout << "[OK]" << endl;
    }
    else {
        cout << "[FAIL] error code: " << r << endl;
        return false;
    }
    return true;
}


/* connects and opens stream to usb */
bool usb_connector::USBConnector::connect(void)
{
    cout << "usb connecting..." << endl;
    if (!init_libusb()) {
        cout << "[FAIL]" << endl;
        return false;
    }
    if (!open_device()) {
        cout << "[FAIL]" << endl;
        return false;
    }
    if (!claim_interface()) {
        cout << "[FAIL]" << endl;
        return false;
    }
    // after finding and opening the device, allocate memory
    // for the transfer objects we will use for reading and writing
    transfer_in  = libusb_alloc_transfer(0);
    transfer_out = libusb_alloc_transfer(0);
    cout << "[OK]" << endl;
    return true;
}


/* reads from the usb stream */
void usb_connector::USBConnector::read(void)
{
    cout << "reading from usb stream..." << endl; 
    // allocate memory for use when reading from the usb
    unsigned char *data = new unsigned char[LEN_IN_BUFFER];
    // prepare a transfer for reading
    libusb_fill_bulk_transfer( transfer_in, usb_dev, USB_ENDPOINT_IN,
        data,  LEN_IN_BUFFER, callback_in, this, 0);
    // submit the transfer, this will fire a request to read from the usb
    int res = libusb_submit_transfer(transfer_in);
    cout << "result code from read transfer: " << res << endl;
    // wait until the transfer is completed
    while (libusb_handle_events_completed(ctx, NULL) != LIBUSB_SUCCESS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 
    // delete the allocated memory
    delete [] data;
}


/* writes to the usb stream */
void usb_connector::USBConnector::write(void)
{
    // get data to send from the send buffer
    vector<unsigned char> vec = bw->readSendBuffer();
    int len = vec.size();
    // check for length, if 0 return
    if (len == 0) return;
    // allocate memory for the data to write, we get a vector
    // from the buffer but need to send char array
    unsigned char *data = new unsigned char[len];
    // copy data from vector to array
    copy(vec.begin(), vec.end(), data);
    cout << "writing to usb stream..." << endl;
    // prepare transfer fro writing
    libusb_fill_bulk_transfer(transfer_out, usb_dev, USB_ENDPOINT_OUT,
        data, len, callback_out, this, 0);
    // submit the transfer
    int res = libusb_submit_transfer(transfer_out);
    cout << "result code from write transfer: " << res << endl;
    // wait for completion
    while (libusb_handle_events_completed(ctx, NULL) != LIBUSB_SUCCESS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }  
    // delete allocated memory
    delete [] data;
}


/* disconnects and closes the usb stream*/
void usb_connector::USBConnector::disconnect(void)
{
    cout << "disconnecting from usb stream... ";
    // release all resources here
    libusb_free_transfer(transfer_in);
    libusb_free_transfer(transfer_out);
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx); 
    cout << "[OK]" << endl;
}


/* handles the callback when reading from the usb stream */
void usb_connector::USBConnector::handle_cb_in(vector<unsigned char> vec)
{
    // call the buffer wrapper and append to is the received data
    bw->appendReceiveBuffer(vec);
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
    vector<unsigned char> vec(transfer->buffer, transfer->buffer + transfer->actual_length);
    connector->handle_cb_in(vec);
}
