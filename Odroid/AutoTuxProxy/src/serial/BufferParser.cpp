
#include <iostream>
#include <string>
#include "serial/BufferParser.h"

using namespace std;

BufferParser::BufferParser() {
    cout << "creating a buffer parser" << endl;
}

BufferParser::~BufferParser() {

}

double BufferParser::decode_packet(string p) {
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
}

int BufferParser::encode_packet() {

}
