/*!
 * Implementation of the BufferWrapper.h. This class takes care
 * of the internal buffers when reading from and writing to the
 * serial connection.
 *
 * @author Ivo
 */

#include <iostream>
#include <iomanip>
#include <mutex>
#include <algorithm>
#include "serial/SerialHandler.h"
#include "containerfactory/SBDContainer.h"

using namespace std;

// append receive buffer mutex
std::mutex arb;
// append send buffer mutex
std::mutex asb;
// read send buffer mutex
std::mutex rsm;
// read receive buffer mutex
std::mutex rrm;

/*! constructor */
serial::SerialBuffer::SerialBuffer(bool a_ver) :
    verbose{a_ver},
    buffer_in{},
    buffer_out{}
{
    cout << "creating serial buffer... ";
    cout << "[OK]" << endl;
}


/*! destructor */
serial::SerialBuffer::~SerialBuffer()
{
    cout << "destroying serial buffer... ";
    cout << "[OK]" << endl;
}


/*! appends a correct packet to the receive buffer */
void serial::SerialBuffer::appendReceiveBuffer(vector<unsigned char> data)
{
    // lock mutex
    arb.lock();
    // return if the length of the vedcor is too short
    if (data.size() < SBDPKTSIZE) {
        // unlock mutex
        arb.unlock();
        return;
    }

    // loop through the received data from the read and look
    // for a correct packet
    for (auto it = data.begin(); it != data.end(); it++) {
        if (it + SBDPKTSIZE > data.end()) {
            break;
        }
        if (*it == DEL_ONE &&
            *(it+DEL_TWO_POS) == DEL_TWO &&
            *(it+DEL_DBCOLON_POS) == DEL_DBCOLON &&
            *(it+DEL_COMMA_POS) == DEL_COMMA) {

            unsigned char us1 = *(it+US1_POS);
            unsigned char us2 = *(it+US2_POS);
            unsigned char ir1 = *(it+IR1_POS);
            unsigned char ir2 = *(it+IR2_POS);
            unsigned char ir3 = *(it+IR3_POS);
            unsigned char wheel = *(it+WHL_POS);
            unsigned char dis1 = *(it+DIS_POS_1);
            unsigned char dis2 = *(it+DIS_POS_2);
            unsigned char dis3 = *(it+DIS_POS_3);
            unsigned char dis4 = *(it+DIS_POS_4);
            unsigned char light = *(it+LIGHT_SEN);
            unsigned char check = *(it+CHK_SUM);

            if (verbose) {
                printf("US1:%i US2:%i IR1:%i IR2:%i IR3:%i WHEEL:%i "
                       "DIS1:%i DIS2:%i DIS3:%i DIS4:%i LIGHT:%i\n",
                       us1, us2, ir1, ir2, ir3, wheel, dis1, dis2, dis3, dis4, light);
            }
            // fill the vector
            vector<unsigned char> valid_pkt =
                {us1, us2, ir1, ir2, ir3, wheel, dis1, dis2, dis3, dis4, light};
            // check if correct checksum
            if (check == checksum(valid_pkt)) {
                buffer_in.clear();
                buffer_in.push_front(valid_pkt);
                break;
            }
            else {
                // find where next packet starts
                it = find(it+1, data.end(), DEL_ONE);
                // if not found, break
                if (it == data.end()) break;
            }
        } else {
            if (verbose) {
                cout << "*** packet fail ***" << endl;
            }
        }
    }
    // unlock mutex
    arb.unlock();
}


/*! returns the most recent valid packet from the read buffer */
vector<unsigned char> serial::SerialBuffer::readReceiveBuffer()
{
    rrm.lock();
    // check for size, i.e. not empty
    if(buffer_in.size() != 0)
    {
        // get 3the most recent packet, always in first position
        std::vector<unsigned char> vec = buffer_in.at(0);
        // clear the buffer
        //buffer_in.clear();
        // put the packet back
        //buffer_in.push_front(vec);
        rrm.unlock();
        return vec;
    }
    else
    {
        rrm.unlock();
        return {};
    }
}


/*! appends a correct packet to the send buffer */
void serial::SerialBuffer::appendSendBuffer(vector<unsigned char> vec)
{
    // lock mutex
    asb.lock();
    buffer_out.clear();
    buffer_out.push_front(vec);
    // and unlock after append
    asb.unlock();
}


/*! returns the most recent valid packet from the send buffer */
vector<unsigned char> serial::SerialBuffer::readSendBuffer()
{
    rsm.lock();
    // check for size, i.e. not empty
    if(buffer_out.size() != 0)
    {
        // get the most recent packet, always in first position
        vector<unsigned char> vec = buffer_out.at(0);
        // clear the buffer
        //buffer_out.clear();
        // put the packet back
        //buffer_out.push_front(vec);
        rsm.unlock();
        return vec;
    }
    else
    {
        rsm.unlock();
        return {};
    }
}


/*! calculates and returns the checksum for a valid packet */
unsigned char serial::SerialBuffer::checksum(const std::vector<unsigned char> pkt)
{
    unsigned char chksum = 0;
    if (pkt.size() == 0) return chksum;
    for (auto it = pkt.begin(); it != pkt.end(); ++it) {
        // the checksum is calculated by XOR all elements
        chksum = chksum ^ *it;
    }
    return chksum;
}
