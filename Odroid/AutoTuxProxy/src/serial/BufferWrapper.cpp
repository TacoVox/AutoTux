
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "serial/BufferWrapper.h"
#include "containerfactory/SBDContainer.h"

using namespace std;


/* constructor */
serial::BufferWrapper::BufferWrapper() : buffer_in({}), buffer_out({})
{
    cout << "creating buffer parser object... ";
    cout << "[OK]" << endl;
}


/* destructor */
serial::BufferWrapper::~BufferWrapper()
{
    cout << "destroying buffer wrapper object... ";
    cout << "[OK]" << endl;
}


/* appends a correct packet to the receive buffer */
void serial::BufferWrapper::appendReceiveBuffer(vector<unsigned char> vec)
{
    // return if the length of the vedcor is too short
    if (vec.size() < 10) return;
    // the vector to hold the correct packet
    vector<unsigned char> v;
    // loop through the received data from the read and look
    // for a correct packet
    for (unsigned int i = 0; i < vec.size(); i++) {
        // if true -> correct packet maybe
        if (vec.at(i) == '7' && vec.at(i+1) == ':' && vec.at(i+9) == ',') {
            cout << "correct packet maybe" << endl;
            unsigned char us1 = vec.at(i+2);
            printf("%i ", us1);
            unsigned char us2 = vec.at(i+3);
            printf("%i ", us2);
            unsigned char ir1 = vec.at(i+4);
            printf("%i ", ir1);
            unsigned char ir2 = vec.at(i+5);
            printf("%i ", ir2);
            unsigned char ir3 = vec.at(i+6);
            printf("%i ", ir3);
            unsigned char wheel = vec.at(i+7);
            printf("%i ", wheel);
            unsigned char check = vec.at(i+8);
            printf("%i \n", check);
            // fill the vector
            v = {us1, us2, ir1, ir2, ir3, wheel};
            // check if correct checksum
            if (check == checksum(v)) {
                cout << "checksum OK" << endl;
                break;
            }
            else {
                cout << "checksum FAIL" << endl;
                // clear the vector
                v.clear();
            }
        }
    } // end for
    // push in front of the buffer if valid data
    if (v.size() != 0) {
        cout << "pushing to buffer in" << endl;
        buffer_in.push_front(v);
    }
}


/* returns the most recent valid packet from the read buffer */
vector<unsigned char> serial::BufferWrapper::readReceiveBuffer(void)
{
    // check for size, i.e. not empty
    if(buffer_in.size() != 0) {
        // get the most recent packet, always in first position
        std::vector<unsigned char> vec = buffer_in.at(0);
        // clear the buffer
        buffer_in.clear();
        // put the packet again so we always have a valid packet
        // with the most recent data to read
        buffer_in.push_front(vec);
        return vec;
    }
    else
        return {};
}


/* appends a correct packet to the send buffer */
void serial::BufferWrapper::appendSendBuffer(vector<unsigned char> vec)
{
    buffer_out.push_front(vec);
}


/* returns the most recent valid packet from the send buffer */
vector<unsigned char> serial::BufferWrapper::readSendBuffer(void)
{
    // check for size, i.e. not empty
    if(buffer_out.size() != 0) {
        // get the most recent packet, always in first position
        vector<unsigned char> v = buffer_out.at(0);
        // clear the buffer
        buffer_out.clear();
        // put the packet again so we always have a valid packet
        // with the most recent data to send
        buffer_out.push_front(v);
        return v;
    }
    else
        return {};
}


/* calculates and returns the checksum for a valid packet */
unsigned char serial::BufferWrapper::checksum(std::vector<unsigned char> vec) {
    unsigned char checksum = 0;
    if (vec.size() == 0) return checksum;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        // the checksum is calculated by XOR all elements
        checksum ^= *it;
    }
    return checksum;
}
