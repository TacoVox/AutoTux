#ifndef BUFFERPARSER_H
#define BUFFERPARSER_H

class BufferParser
{
public:
    BufferParser();
    ~BufferParser();
    double decode_packet(unsigned char *p);
    int encode_packet(void);
};

#endif // BUFFERPARSER_H
