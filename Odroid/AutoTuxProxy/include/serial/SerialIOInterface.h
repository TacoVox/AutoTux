/*!
 * USBConnector header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_SERIALIOINTERFACE_H
#define AUTOTUXPROXY_SERIALIOINTERFACE_H


#include <vector>

namespace serial
{
    class SerialIOInterface
    {
    public:
        virtual ~SerialIOInterface();
        virtual bool connect() = 0;
        virtual bool disconnect() = 0;
        virtual int read(unsigned char *, int *) = 0;
        virtual int write(std::vector<unsigned char>) = 0;
    };
} // namespace serial

#endif	// AUTOTUXPROXY_SERIALIOINTERFACE_H

