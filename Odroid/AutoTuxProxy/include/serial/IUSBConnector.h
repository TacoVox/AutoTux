/*!
 * USBConnector interface.
 *
 * @author Ivo
 */


#ifndef IUSBCONNECTOR_H
#define IUSBCONNECTOR_H

class IUSBConnector
{
public:
    /*! destructor */
    virtual ~IUSBConnector();
    /*! connects to usb */
    virtual bool connect(void) = 0;
    /*! disconnects and closes the usb stream */
    virtual bool disconnect(void) = 0;
    /*! reads from the usb stream */
    virtual int read(void) = 0;
    /*! writes to the usb stream */
    virtual int write(void) = 0;
};


#endif	// IUSBCONNECTOR_H

