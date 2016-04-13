
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "serial/BufferWrapper.h"
#include "containerfactory/SBDContainer.h"

using namespace std;


serial::BufferWrapper::BufferWrapper() :
buffer_in({}), buffer_out({})
{
    cout << "creating buffer parser object" << endl;
}


serial::BufferWrapper::~BufferWrapper()
{
    cout << "destroying buffer parser object" << endl;
}


void serial::BufferWrapper::appendReceiveBuffer(unsigned char *cin, int len)
{
    if (len < 9) return;
    //cout << "buffer read: " << len << endl;
    vector<unsigned char> v;
    for (int i = 0; i < len; i++) {
        printf("%i\n", cin[i]);
        // if true -> correct packet ?maybe
        if (cin[i] == '7' && cin[i+1] == ':' && cin[i+9] == ',') {
            cout << "correct packet maybe" << endl;
            unsigned char us1 = cin[i+2];
            printf("%i ", us1);
            unsigned char us2 = cin[i+3];
            printf("%i ", us2);
            unsigned char ir1 = cin[i+4];
            printf("%i ", ir1);
            unsigned char ir2 = cin[i+5];
            printf("%i ", ir2);
            unsigned char ir3 = cin[i+6];
            printf("%i ", ir3);
            unsigned char wheel = cin[i+7];
            printf("%i ", wheel);
            unsigned char check = cin[i+8];
            printf("%i \n", check);
            v = {us1, us2, ir1, ir2, ir3, wheel};
            // for now
            if (check == checksum(v)) {
                cout << "checksum OK" << endl;
                break;
            }
            else {
                cout << "checksum FAIL" << endl;
            }
        }
    } // end for

    // push in front of the deque holding data (aka 'the buffer')
    buffer_in.push_front(v);
}


vector<unsigned char> serial::BufferWrapper::readReceiveBuffer(void)
{
    std::vector<unsigned char> vec = buffer_in.at(0);
    buffer_in.clear();
    buffer_in.push_front(vec);
    return vec;
}


void serial::BufferWrapper::appendSendBuffer(vector<unsigned char> vec)
{
    buffer_out.push_front(vec);
}


//Here we will need to return a packet to the calling function.
vector<unsigned char> serial::BufferWrapper::readSendBuffer(void)
{
    if(buffer_out.size() != 0) {
        vector<unsigned char> v = buffer_out.at(0);
        buffer_out.clear();
        buffer_out.push_front(v);
        return v;
    }
    else
        return vector<unsigned char> {0};
}


unsigned char serial::BufferWrapper::checksum(std::vector<unsigned char> vec)
{
    unsigned char checksum = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        checksum ^= *it;
    }
    return checksum;
}

vector<double> serial::decode_packet(string p, int len)
{
    vector<double> v;
    return v;
}

int serial::encode_packet()
{
    return 0;
}
