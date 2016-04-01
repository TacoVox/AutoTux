#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H
// included dependencies
// ============================
#include <libusb-1.0/libusb.h>

// define SerialConnector class
// ============================
class SerialConnector
{
public:
    // constructor
    SerialConnector();
    // copy constructor
    SerialConnector(const SerialConnector &sc);
    // assignment
    SerialConnector & operator=(const SerialConnector &sc);
    // destructor
    ~SerialConnector();
    void connect();
    void read();
    void write(unsigned char c);
    void disconnect();
    void printdev(int i);
    ssize_t num_devices();
private:
    libusb_device **devs;
    libusb_device_handle *usb_dev;
    libusb_context *ctx;
    libusb_device_descriptor desc;
    libusb_config_descriptor *config;
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
};

#endif	// SERIALCONNECTOR_H

