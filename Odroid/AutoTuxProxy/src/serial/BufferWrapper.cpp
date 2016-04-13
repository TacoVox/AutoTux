
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

    vector<unsigned char> v;
    for (int i = 0; i < len; i++) {
        cout << cin[i] << endl;
        // if true -> correct packet ?maybe
        if (cin[i] == '6' && cin[i+1] == ':' && cin[i+8] == ',') {
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
            unsigned char check = cin[i+7];
            printf("%i \n", check);
            // for now
            if (check == 0x00) {
                v = {us1, us2, ir1, ir2, ir3};
                break;
            }
            // check for the checksum here
            //unsigned char check = cin[i+7];
            //if (checksum(v) == check) {
                // found correct packet
            //    break;
            //}
        }
        else {
            cout << "received incorrect packet" << endl;
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


void serial::BufferWrapper::appendSendBuffer(vector<unsigned char>)
{
    //send_buffer.insert(0, str);
}


//Here we will need to return a packet to the calling function.
vector<unsigned char> serial::BufferWrapper::readSendBuffer(void)
{
    if(buffer_out.size() != 0)
        return buffer_out.at(0);
    else
        return vector<unsigned char> {0, 1};
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

}
