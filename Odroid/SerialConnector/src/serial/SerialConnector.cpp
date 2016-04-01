
#include <iostream>
#include "SerialConnector.h"

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
}
