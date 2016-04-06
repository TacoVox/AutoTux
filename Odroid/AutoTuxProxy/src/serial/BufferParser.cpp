
#include <iostream>
#include <string>
#include <vector>
#include "serial/BufferParser.h"

using namespace std;


vector<double> buf_parser::decode_packet(string p, int len) {
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

int buf_parser::encode_packet() {

}
