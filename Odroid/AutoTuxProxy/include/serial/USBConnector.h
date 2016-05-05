/*!
 * USBConnector header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_USBCONNECTOR_H
#define AUTOTUXPROXY_USBCONNECTOR_H


namespace serial
{
    namespace conninter
    {
        class USBConnector
        {
        public:
            virtual ~USBConnector();
            virtual bool connect() = 0;
            virtual bool disconnect() = 0;
            virtual int read() = 0;
            virtual int write() = 0;
        };
    } // namespace conninter
} // namespace serial

#endif	// AUTOTUXPROXY_USBCONNECTOR_H

