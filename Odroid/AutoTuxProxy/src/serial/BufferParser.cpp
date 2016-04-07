
#include <iostream>
#include "serial/BufferParser.h"
#include "containerfactory/SBDContainer.h"

using namespace std;

buf_parser::BufferParser::BufferParser()
{
    cout << "creating buffer parser object" << endl;
    buffer = "";
}

buf_parser::BufferParser::~BufferParser()
{
    cout << "destroying buffer parser object" << endl;
}

void buf_parser::BufferParser::submit(string str)
{
    buffer.insert(0, str);
}

void buf_parser::BufferParser::read_buffer(void)
{
    // put this in a loop as well
    if (isalpha(buffer[0]) && (short)buffer[0] == 6) {
        unsigned char us1 = buffer[2];
        unsigned char us2 = buffer[3];
        unsigned char ir1 = buffer[4];
        unsigned char ir2 = buffer[5];
        unsigned char ir3 = buffer[6];
        std::vector<unsigned char> vec{us1, us2, ir1, ir2, ir3};
        if (checksum(vec) == buffer[7]) {
            // correct checksum, send to broadcast
            containerfactory::SBDContainer::instance()->genSBDContainer(vec);
        }
        else {
            // checksum not correct, continue reading
            buffer.clear();
        }
    }
}

unsigned char buf_parser::BufferParser::checksum(std::vector<unsigned char> vec)
{
    unsigned char checksum = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        checksum ^= *it;
    }
    return checksum;
}

vector<double> buf_parser::decode_packet(string p, int len)
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

int buf_parser::encode_packet()
{

}
