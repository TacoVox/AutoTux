#ifndef BUFFERPARSER_H
#define BUFFERPARSER_H

#include <string>

class BufferParser
{
public:
    BufferParser();
    ~BufferParser();
    double decode_packet(std::string str);
    int encode_packet(void);
};

#endif // BUFFERPARSER_H
