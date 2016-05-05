/*!
 * USBConnector header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_USBCONNECTOR_H
#define AUTOTUXPROXY_USBCONNECTOR_H


namespace serial
{
    class SerialIOInterface
    {
    public:
        virtual ~SerialIOInterface();
        virtual bool connect() = 0;
        virtual bool disconnect() = 0;
        virtual int read() = 0;
        virtual int write() = 0;
    };
} // namespace serial

#endif	// AUTOTUXPROXY_USBCONNECTOR_H

