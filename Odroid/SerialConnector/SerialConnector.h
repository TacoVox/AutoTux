
// included dependencies
// =============================================================================
#include <libusb-1.0/libusb.h>

// define SerialConnector class
// =============================================================================
class SerialConnector
{
public:
	SerialConnector();
	void connect();
	void read();
	void write(unsigned char byte);
	void disconnect();
	~SerialConnector();
private:
	libusb_device **devs;
	libusb_device_handle *usb_dev;
	libusb_context *ctx = NULL;
};
