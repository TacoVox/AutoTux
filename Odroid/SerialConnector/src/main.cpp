#include <iostream>
#include "serial/SerialConnector.h"
#include <libusb-1.0/libusb.h>

using namespace std;

int main() {
    
    SerialConnector serial_obj;
    serial_obj.connect();
    ssize_t i; //for iterating through the list
    for(i = 0; i < serial_obj.num_devices(); i++) {
        serial_obj.printdev(i); //print specs of this device
    }
    serial_obj.disconnect();
    return 0;
}
