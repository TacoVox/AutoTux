/**
 * Implementation of the USBConnector.h. Responsible for reading and
 * writing from/to the usb serial connection.
 *
 * @author Ivo
 */

#include <iostream>
#include <thread>
#include "serial/SerialIOImpl.h"

using namespace std;


/*! constructor */
serial::SerialIOImpl::SerialIOImpl() :
    ctx{},
    usb_dev{}
{
    cout << "creating usb connector... ";
    libusb_init(&ctx);
    cout << "[OK]" << endl;
}


/*! copy constructor */
serial::SerialIOImpl::SerialIOImpl(const serial::SerialIOImpl &usb) :
    ctx(usb.ctx),
    usb_dev(usb.usb_dev)
{}


/*! copy constructor */
serial::SerialIOImpl &
serial::SerialIOImpl::operator=(const serial::SerialIOImpl &usb)
{
    this->ctx = usb.ctx;
    this->usb_dev = usb.usb_dev;
    return *this;
}


/*! destructor */
serial::SerialIOImpl::~SerialIOImpl()
{
    cout << "destroying serial io... ";
    // release all resources here
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    libusb_exit(ctx);
    cout << "[OK]" << endl;
}


/**
 * Required by the interface.
 */
serial::SerialIOInterface::~SerialIOInterface() {}


/*! gets a list of the devices and opens the one we need */
bool serial::SerialIOImpl::open_device()
{
    // to return
    bool result{false};
    // the devices available list
    struct libusb_device **devs;
    // get available devices
    ssize_t dev_count = libusb_get_device_list(ctx, &devs);
    cout << "opening device... ";
    // go through the devices list searching for the one we need
    for (ssize_t i = 0; i < dev_count; i++) {
        struct libusb_device_descriptor dev_desc;
        // for each device in the list, get its descriptor
        int r = libusb_get_device_descriptor(devs[i], &dev_desc);
        if (r < 0) {
            cout << "[FAIL] dev_desc" << endl;
        }
        else if (dev_desc.idVendor == USB_VENDOR_ID &&
                    dev_desc.idProduct == USB_PRODUCT_ID) {
            // if the device matches, open it
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


/*! claims the conninter of the USB for I/O operations */
bool serial::SerialIOImpl::claim_interface()
{
    // to return
    bool result{false};
    // check if the kernel is active on the device and
    // the interface we want to operate on
    if (libusb_kernel_driver_active(usb_dev, 1) == 1) {
        // detach kernel if active
        libusb_detach_kernel_driver(usb_dev, 1);
    }
    cout << "claiming interface... ";
    // claim the conninter for the device, this will let us
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


/*! connects to usb */
bool serial::SerialIOImpl::connect()
{
    cout << "usb connecting..." << endl;
    if (open_device()) {
        if (claim_interface()) {
            cout << "[OK]" << endl;
            return true;
        }
    }

    cout << "[FAIL]" << endl;
    return false;
}


/*! reads from the usb stream */
int serial::SerialIOImpl::read(unsigned char *data, int *transferred)
{
    // allocate memory for use when reading from the usb
    //unsigned char *data = new unsigned char[READ_LEN];
    // actual bytes read
    //int transferred;
    // pass to the transfer the usb device, the endpoint (read in),
    // the char array to store the data read, the size, an int for
    // the actual bytes read and the timeout for the operation
    int res = libusb_bulk_transfer(usb_dev, USB_ENDPOINT_IN,
                                   data, READ_LEN, transferred, 20);
    /*
    if (res == 0) {
        // the vector holding the data from the read
        vector<unsigned char> vec(data, data + transferred);
        // append to the receive buffer
        pserialbuf->appendReceiveBuffer(vec);
    }
     */
    // delete the allocated memory
    //delete [] data;
    // return the result from the read
    return res;
}


/*! writes to the usb stream */
int serial::SerialIOImpl::write(vector<unsigned char> vec)
{
    // get data from the send buffer
    //vector<unsigned char> vec = pserialbuf->readSendBuffer();
    // get length
    long unsigned int len{vec.size()};
    // check for length, if 0 return
    if (len == 0) return EMPTY_DATA;
    // allocate memory for the data to write, we get a vector
    // from the buffer but need to send char array
    unsigned char *data = new unsigned char[len];
    // copy data from vector to array
    copy(vec.begin(), vec.end(), data);
    // actual bytes written
    int transferred;
    // pass to the transfer the usb device, the endpoint (write out),
    // the char array to write, the size, an int for
    // the actual bytes written and the timeout for the operation
    int res = libusb_bulk_transfer(usb_dev, USB_ENDPOINT_OUT,
                                   data, (unsigned int)len, &transferred, 20);
    // delete allocated memory
    delete [] data;
    // return result from the write
    return res;
}


/*! disconnects and closes the usb stream */
bool serial::SerialIOImpl::disconnect()
{
    cout << "disconnecting from usb stream... ";
    // release resources here
    libusb_release_interface(usb_dev, 1);
    libusb_attach_kernel_driver(usb_dev, 1);
    libusb_close(usb_dev);
    cout << "[OK]" << endl;

    return true;
}

