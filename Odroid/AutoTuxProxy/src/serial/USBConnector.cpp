
#include <iostream>
#include <thread>
#include <chrono>
#include "serial/USBConnector.h"

using namespace std;


/* constructor */
usb_connector::USBConnector::USBConnector()
{
    cout << "creating a usb connector object... ";
    cout << "[OK]" << endl;
}


/* copy constructor */
usb_connector::USBConnector::USBConnector(const usb_connector::USBConnector &usb) :
    bw{usb.bw},
    ctx{usb.ctx},
    usb_dev{usb.usb_dev}
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
    cout << "destroying usb connector... ";
    // release all resources here
    bw->~BufferWrapper();
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx);
    cout << "[OK]" << endl;
}


void usb_connector::USBConnector::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> p)
{
    bw = p;
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


/* gets a list of the devices and opens the one we need */
bool usb_connector::USBConnector::open_device(void)
{
    // to return
    bool result = false;
    // the devices available list
    struct libusb_device **devs;
    // get available devices
    ssize_t dev_count = libusb_get_device_list(ctx, &devs);
    cout << "finding devices... ";
    if (dev_count < 0) {
        cout << "[FAIL] error code: " << dev_count << endl;
    }
    else {
        cout << "[OK] " << endl;
    }
    cout << "opening device... ";
    // go through the devices list searching for the one we need
    for (ssize_t i = 0; i < dev_count; i++) {
        libusb_device_descriptor desc;
        // for each device in the list, get its descriptor
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout << "[FAIL] dev_desc" << endl;
        }
        // if the device matches, open it
        if (desc.idVendor == USB_VENDOR_ID && desc.idProduct == USB_PRODUCT_ID) {
            int open = libusb_open(devs[i], &usb_dev);
            if (open != 0) {
                cout << "[FAIL] error code: " << open << endl;
            }
            else {
                cout << "[OK]" << endl;
                result = true;
            }
            break;
        }
    }
    // free the devices list when done with it
    libusb_free_device_list(devs, 1);
    return result;
}


/* claims the interface of the USB for I/O operations */
bool usb_connector::USBConnector::claim_interface(void)
{
    // to return
    bool result = false;
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
    }
    cout << "claiming interface... ";
    // claim the interface for the device, this will let us
    // send and receive data
    int r = libusb_claim_interface(usb_dev, 1);
    if (r == 0) {
        cout << "[OK]" << endl;
        result = true;
    }
    else {
        cout << "[FAIL] error code: " << r << endl;
    }
    return result;
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
    cout << "[OK]" << endl;
    return true;
}


/* reads from the usb stream */
int usb_connector::USBConnector::read(void)
{
    cout << "reading from usb stream..." << endl; 
    // allocate memory for use when reading from the usb
    unsigned char *data = new unsigned char[READ_LEN];
    // read from the usb device
    int transferred;
    int res = libusb_bulk_transfer(usb_dev, USB_ENDPOINT_IN,
                                   data, READ_LEN, &transferred, 20);
    if (res == 0) {
        cout << "READ successful" << endl;
        cout << "bytes received: " << transferred << endl;
        // the vector holding the data from the read
        vector<unsigned char> vec(data, data + transferred);
        // append to the receive buffer
        bw->appendReceiveBuffer(vec);
    }
    // delete the allocated memory
    delete [] data;
    return res;
}


/* writes to the usb stream */
int usb_connector::USBConnector::write(void)
{
    // get data from the buffer
    vector<unsigned char> vec = bw->readSendBuffer();
    // get data to send from the send buffer
    int len = vec.size();
    // check for length, if 0 return
    if (len == 0) return EMPTY_DATA;
    // allocate memory for the data to write, we get a vector
    // from the buffer but need to send char array
    unsigned char *data = new unsigned char[len];
    // copy data from vector to array
    copy(vec.begin(), vec.end(), data);
    cout << "writing to usb stream..." << endl;
    int transferred;
    int res = libusb_bulk_transfer(usb_dev, USB_ENDPOINT_OUT,
                                   data, len, &transferred, 20);
    if (res == 0) {
        cout << "WRITE successful" << endl;
        cout << "bytes sent: " << transferred << endl;
    }
    // delete allocated memory
    delete [] data;
    return res;
}


/* disconnects and closes the usb stream*/
void usb_connector::USBConnector::disconnect(void)
{
    cout << "disconnecting from usb stream... ";
    // release resources here
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx); 
    cout << "[OK]" << endl;
}

