#include <iostream>
#include "serial/BufferWrapper.h"
#include "containerfactory/SBDContainer.h"

using namespace std;


serial::BufferWrapper::BufferWrapper() : receive_buffer(""), send_buffer("")
{
    cout << "creating buffer parser object" << endl;
}


serial::BufferWrapper::~BufferWrapper()
{
    cout << "destroying buffer parser object" << endl;
}


void serial::BufferWrapper::appendReceiveBuffer(string str)
{
    receive_buffer.insert(0, str);
}


string serial::BufferWrapper::readReceiveBuffer(void)
{
    // put this in a loop as well
    if (isalpha(receive_buffer[0]) && (short)receive_buffer[0] == 6) {
        unsigned char us1 = receive_buffer[2];
        unsigned char us2 = receive_buffer[3];
        unsigned char ir1 = receive_buffer[4];
        unsigned char ir2 = receive_buffer[5];
        unsigned char ir3 = receive_buffer[6];
        std::vector<unsigned char> vec{us1, us2, ir1, ir2, ir3};
        if (checksum(vec) == receive_buffer[7]) {
            // correct checksum, send to broadcast
            containerfactory::SBDContainer::instance()->genSBDContainer(vec);
        }
        else {
            // checksum not correct, continue reading
            receive_buffer.clear();
        }
    }
}


void serial::BufferWrapper::appendSendBuffer(string str)
{
    send_buffer.insert(0, str);
}


//Here we will need to return a packet to the calling function.
string serial::BufferWrapper::readSendBuffer(void)
{

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
    /*
    cout << "decoding packet..." << endl;
    string::size_type del = p.find(':');
    del += 2;
    cout << "first : found at " << del << endl;
    string sub1 = p.substr(del, p.length());
    cout << "sub1-" << sub1 << endl;
    string::size_type del1 = sub1.find(' ');
    cout << "white space found at " << del1 << endl;
    string sub2 = sub1.substr(0, del1);
    cout << "sub2-" << sub2 << endl;
    return stod(sub2);
    */
    vector<double> v;
    return v;
}

int serial::encode_packet()
{

}
