
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
private:
    libusb_device **devs;
    libusb_device_handle *usb_dev;
    libusb_context *ctx;
};


