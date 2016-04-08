
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "serial/BufferWrapper.h"
#include "containerfactory/SBDContainer.h"

using namespace std;


serial::BufferWrapper::BufferWrapper()
{
    cout << "creating buffer parser object" << endl;
}


serial::BufferWrapper::~BufferWrapper()
{
    cout << "destroying buffer parser object" << endl;
}


void serial::BufferWrapper::appendReceiveBuffer(unsigned char *cin, int len)
{
    //buffer_in.insert(buffer_in.begin(), *cin, len);
    cout << "len is: " << len << endl;
    /*for (int i = 0; i < len; i++) {
        printf("%i: ", cin[i]);
    }*/
    //auto it = buffer_in.begin();
    for (int i = 0; i < len; i++) {
        buffer_in.push_back(cin[i]);
        //*it = cin[i];
        //it++;
    }
    //cout << endl;
    cout << "buffer size before read: " << buffer_in.size() << endl;
}


vector<unsigned char> serial::BufferWrapper::readReceiveBuffer(void)
{
    // put this in a loop as well
    /*if (isalpha(receive_buffer[0]) && (short)receive_buffer[0] == 6) {
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
    }*/

    cout << "reading from buffer in the wrapper class" << endl;
    cout << "buffer size is: " << buffer_in.size() << endl;
    std::vector<unsigned char> vec;
    for (int i = 0; i < buffer_in.size(); i++) {
        if (buffer_in.at(i) == '6' &&  buffer_in.at(i+1) == ':') {
            unsigned char us1 = buffer_in.at(i+2);
            unsigned char us2 = buffer_in.at(i+3);
            unsigned char ir1 = buffer_in.at(i+4);
            unsigned char ir2 = buffer_in.at(i+5);
            unsigned char ir3 = buffer_in.at(i+6);
            cout << "filling vector in buffer wrapper" << endl;
            vec = {us1, us2, ir1, ir2, ir3};
            cout << "ok" << endl;
            break;
        }
    }
    cout << "clearing buffer in the wrapper class" << endl;
    buffer_in.clear();
    cout << "buffer cleared" << endl;
    return vec;
}


void serial::BufferWrapper::appendSendBuffer(string str)
{
    //send_buffer.insert(0, str);
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
