#ifndef BUFFERPARSER_H
#define BUFFERPARSER_H

#include <string>
#include <vector>


namespace buf_parser {
    std::vector<double> decode_packet(std::string str, int len);
    int encode_packet(void);
}


#endif // BUFFERPARSER_H
