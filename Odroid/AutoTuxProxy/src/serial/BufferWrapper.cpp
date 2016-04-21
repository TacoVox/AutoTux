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
#include "serial/BufferWrapper.h"
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
serial::BufferWrapper::BufferWrapper() : buffer_in({}), buffer_out({})
{
    cout << "creating buffer wrapper... ";
    cout << "[OK]" << endl;
}


/*! destructor */
serial::BufferWrapper::~BufferWrapper()
{
    cout << "destroying buffer wrapper... ";
    cout << "[OK]" << endl;
}


/*! appends a correct packet to the receive buffer */
void serial::BufferWrapper::appendReceiveBuffer(vector<unsigned char> vec)
{
    // lock mutex
    arb.lock();
    // return if the length of the vedcor is too short
    if (vec.size() < SBDPKTSIZE) {
        // unlock mutex
        arb.unlock();
        return;
    }
    // the vector to hold the correct packet
    vector<unsigned char> ret_vec;
    // loop through the received data from the read and look
    // for a correct packet
    for (auto it = vec.begin(); it != vec.end(); it++) {
        if (it + SBDPKTSIZE > vec.end()) {
            break;
        }
        if (*it == STR_DEL && *(it+STR_DEL_POS) == STR_DEL &&
                *(it+MID_DEL_POS) == MID_DEL && *(it+END_DEL_POS) == END_DEL) {
            unsigned char us1 = *(it+US1_POS);
            printf("US1:%i ", us1);
            unsigned char us2 = *(it+US2_POS);
            printf("US2:%i ", us2);
            unsigned char ir1 = *(it+IR1_POS);
            printf("IR1:%i ", ir1);
            unsigned char ir2 = *(it+IR2_POS);
            printf("IR2:%i ", ir2);
            unsigned char ir3 = *(it+IR3_POS);
            printf("IR3:%i ", ir3);
            unsigned char wheel = *(it+WHL_POS);
            printf("WHEEL:%i ", wheel);
            unsigned char dis1 = *(it+DIS_POS_1);
            printf("DIS1:%i ", dis1);
            unsigned char dis2 = *(it+DIS_POS_2);
            printf("DIS2:%i ", dis2);
            unsigned char dis3 = *(it+DIS_POS_3);
            printf("DIS3:%i ", dis3);
            unsigned char dis4 = *(it+DIS_POS_4);
            printf("DIS4:%i ", dis4);
            unsigned char check = *(it+CHK_SUM);
            printf("CHECK:%i\n", check);
            // fill the vector
            ret_vec = {us1, us2, ir1, ir2, ir3, wheel, dis1, dis2, dis3, dis4};
            // check if correct checksum
            if (check == checksum(ret_vec)) {
                cout << "checksum OK" << endl;
                break;
            }
            else {
                cout << "checksum FAIL" << endl;
                // clear the return vector
                ret_vec.clear();
                // find where next packet starts
                it = find(it+1, vec.end(), '7');
                // if not found, break
                if (it == vec.end()) break;
            }
        }
    }
    // push in front of the buffer if valid data
    if (ret_vec.size() != 0) {
        buffer_in.push_front(ret_vec);
    }
    // unlock mutex
    arb.unlock();
}


/*! returns the most recent valid packet from the read buffer */
vector<unsigned char> serial::BufferWrapper::readReceiveBuffer(void)
{
    rrm.lock();
    // check for size, i.e. not empty
    if(buffer_in.size() != 0)
    {
        // get the most recent packet, always in first position
        std::vector<unsigned char> vec = buffer_in.at(0);
        // clear the buffer
        buffer_in.clear();
        // put the packet again so we always have a valid packet
        // with the most recent data to read
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
void serial::BufferWrapper::appendSendBuffer(vector<unsigned char> vec)
{
    // lock mutex
    asb.lock();
    buffer_out.push_front(vec);
    // and unlock after append
    asb.unlock();
}


/*! returns the most recent valid packet from the send buffer */
vector<unsigned char> serial::BufferWrapper::readSendBuffer(void)
{
    rsm.lock();
    // check for size, i.e. not empty
    if(buffer_out.size() != 0)
    {
        // get the most recent packet, always in first position
        vector<unsigned char> v = buffer_out.at(0);
        // clear the buffer
        buffer_out.clear();
        // put the packet again so we always have a valid packet
        // with the most recent data to send
        //buffer_out.push_front(v);
        rsm.unlock();
        return v;
    }
    else
    {
        rsm.unlock();
        return {};
    }
}


/*! calculates and returns the checksum for a valid packet */
unsigned char serial::BufferWrapper::checksum(std::vector<unsigned char> vec)
{
    unsigned char chksum = 0;
    if (vec.size() == 0) return chksum;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        // the checksum is calculated by XOR all elements
        chksum = (unsigned char)(chksum ^ *it);
    }
    return chksum;
}
