/*!
 * Implementation of the USBConnector.h. Responsible for reading and
 * writing from/to the usb serial connection.
 *
 * @author Ivo
 */

#include <iostream>
#include <thread>
#include <chrono>
#include "serial/USBConnector.h"

using namespace std;


/*! constructor */
usb_connector::USBConnector::USBConnector() :
    bw{},
    ctx{},
    usb_dev{}
{
    cout << "creating usb connector... ";
    cout << "[OK]" << endl;
}


/*! copy constructor */
usb_connector::USBConnector::USBConnector(const usb_connector::USBConnector &usb) :
    bw{usb.bw},
    ctx{usb.ctx},
    usb_dev{usb.usb_dev}
{}


/*! copy constructor */
usb_connector::USBConnector &
usb_connector::USBConnector::operator=(const usb_connector::USBConnector &usb)
{
    bw = usb.bw;
    ctx = usb.ctx;
    usb_dev = usb.usb_dev;
    return *this;
}


/*! destructor */
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


/*! sets the buffer wrapper for this connector */
void usb_connector::USBConnector::set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper> ptr)
{
    cout << "setting buffer wrapper to usb connector" << endl;
    bw = ptr;
}


/*! initializes libusb */
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


/*! gets a list of the devices and opens the one we need */
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


/*! claims the interface of the USB for I/O operations */
bool usb_connector::USBConnector::claim_interface(void)
{
    // to return
    bool result = false;
    // check if the kernel is active on the device and
    // the interface we want to operate on
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


/*! connects to usb */
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


/*! reads from the usb stream */
int usb_connector::USBConnector::read(void)
{
    // allocate memory for use when reading from the usb
    unsigned char *data = new unsigned char[READ_LEN];
    // actual bytes read
    int transferred;
    // pass to the transfer the usb device, the endpoint (read in),
    // the char array to store the data read, the size, an int for
    // the actual bytes read and the timeout for the operation
    int res = libusb_bulk_transfer(usb_dev, USB_ENDPOINT_IN,
                                   data, READ_LEN, &transferred, 20);
    if (res == 0) {
        cout << "bytes read: " << transferred << endl;
        // the vector holding the data from the read
        vector<unsigned char> vec(data, data + transferred);
        // append to the receive buffer
        bw->appendReceiveBuffer(vec);
    }
    // delete the allocated memory
    delete [] data;
    // return the result from the read
    return res;
}


/*! writes to the usb stream */
int usb_connector::USBConnector::write(void)
{
    // get data from the send buffer
    vector<unsigned char> vec = bw->readSendBuffer();
    // get length
    long unsigned int len = vec.size();
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
    if (res == 0) {
        cout << "bytes sent: " << transferred << endl;
    }
    // delete allocated memory
    delete [] data;
    // return result from the write
    return res;
}


/*! disconnects and closes the usb stream */
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

