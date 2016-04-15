
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "serial/BufferWrapper.h"
#include "containerfactory/SBDContainer.h"

using namespace std;

serial::BufferWrapper::BufferWrapper() :
buffer_in({}), buffer_out({}) {
    cout << "creating buffer parser object... ";
    cout << "[OK]" << endl;
}


serial::BufferWrapper::~BufferWrapper() {
    cout << "destroying buffer wrapper object... ";
    cout << "[OK]" << endl;
}


void serial::BufferWrapper::appendReceiveBuffer(vector<unsigned char> vec) {
    if (vec.size() < 10) return;

    vector<unsigned char> v;
    for (unsigned int i = 0; i < vec.size(); i++) {
        // if true -> correct packet ?maybe
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
            v = {us1, us2, ir1, ir2, ir3, wheel};
            // for now
            if (check == checksum(v)) {
                cout << "checksum OK" << endl;
                break;
            }
            else {
                cout << "checksum FAIL" << endl;
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


vector<unsigned char> serial::BufferWrapper::readReceiveBuffer(void) {
    if(buffer_in.size() != 0) {
        std::vector<unsigned char> vec = buffer_in.at(0);
        buffer_in.clear();
        buffer_in.push_front(vec);
        return vec;
    } else {
        return {};
    }
}


void serial::BufferWrapper::appendSendBuffer(vector<unsigned char> vec) {
    buffer_out.push_front(vec);
}


//Here we will need to return a packet to the calling function.
vector<unsigned char> serial::BufferWrapper::readSendBuffer(void) {
    if(buffer_out.size() != 0) {
        vector<unsigned char> v = buffer_out.at(0);
        buffer_out.clear();
        buffer_out.push_front(v);
        return v;
    }
    else
        return {};
}


unsigned char serial::BufferWrapper::checksum(std::vector<unsigned char> vec) {
    unsigned char checksum = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        checksum ^= *it;
    }
    return checksum;
}
