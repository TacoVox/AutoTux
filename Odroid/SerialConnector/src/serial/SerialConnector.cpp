
#include <iostream>
#include "SerialConnector.h"
#include <libusb-1.0/libusb.h>

using namespace std;

/* constructor */
SerialConnector::SerialConnector() {
    cout << "Creating a serial object!" << endl;
}

/* copy constructor */
SerialConnector::SerialConnector(const SerialConnector &sc) {

}

/* assignment */
SerialConnector & SerialConnector::operator=(const SerialConnector &sc) {

}

/* destructor */
SerialConnector::~SerialConnector() {
    delete[] devs;
    free(usb_dev);
    free(ctx);
}

/* connect and open stream to usb */
void SerialConnector::connect() {
    cout << "Connecting to usb stream..." << endl;
    r = libusb_init(&ctx); //initialize a library session
    if(r < 0) {
        cout << "Init Error " << r << endl; //there was an error
        return;
    }
    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        cout << "Get Device Error" << endl; //there was an error
    }
    cout << cnt << " Devices in list." << endl; //print total number of usb devices
}

/* read from the usb stream */
void SerialConnector::read() {
    cout << "Reading from usb stream..." << endl;
}

/* write to the usb stream */
void SerialConnector::write(unsigned char c) {
    cout << "Writing to usb stream..." << endl;
}

/* disconnect and close the usb stream*/
void SerialConnector::disconnect() {
    cout << "Disconnecting from usb stream..." << endl;
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
    libusb_exit(ctx); //close the session
}

void SerialConnector::printdev(int i) {
    int r = libusb_get_device_descriptor(devs[i], &desc);
    
    if (r < 0) {
        cout << "failed to get device descriptor" << endl;
        return;
    }
    
    cout << "Number of possible configurations: " << (int)desc.bNumConfigurations << " ";
    cout << "Device Class: " << (int) desc.bDeviceClass << " ";
    cout << "VendorID: " << desc.idVendor << " ";
    cout << "ProductID: " << desc.idProduct << endl;
    
    libusb_get_config_descriptor(devs[i], 0, &config);
    cout << "Interfaces: " << (int) config->bNumInterfaces << " ||| ";
    
    for(int i = 0; i < (int) config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        cout << "Number of alternate settings: " << inter->num_altsetting << " | ";
        
        for(int j = 0; j < inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            cout << "Interface Number: " << (int) interdesc->bInterfaceNumber << " | ";
            cout << "Number of endpoints: " << (int) interdesc->bNumEndpoints << " | ";
            
            for(int k = 0; k < (int) interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                cout << "Descriptor Type: " << (int) epdesc->bDescriptorType << " | ";
                cout << "EP Address: " << (int) epdesc->bEndpointAddress << " | ";
            }
        }
    }
    cout << endl << endl << endl;
    libusb_free_config_descriptor(config);
}

ssize_t SerialConnector::num_devices() {
    return cnt;
}

