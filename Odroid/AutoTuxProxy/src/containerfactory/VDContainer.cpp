/*
 * Class for creating VehicleData Objects and wrapping them into a container.
 */
#include "containerfactory/VDContainer.h"
#include <automotivedata/generated/automotive/VehicleData.h>
#include <iostream>

using namespace odcore::data;
using namespace automotive;

containerfactory::VDContainer* containerfactory::VDContainer::_instance = 0;

//Private Constructors & Destructor so that just one element can exist.
//And this very one should be created by the class itself.
containerfactory::VDContainer::VDContainer() {}
containerfactory::VDContainer::VDContainer(const VDContainer &) {}
containerfactory::VDContainer::~VDContainer() {}

//Returns an instance of that (singelton)class
containerfactory::VDContainer* containerfactory::VDContainer::instance(void) {
    if (!_instance)
        _instance = new containerfactory::VDContainer();
    return _instance;
}

//Method for creating a the very Object and returning a (shared) pointer to it.
std::shared_ptr<Container> containerfactory::VDContainer::genVDContainer(
        std::vector<unsigned char> v) {
    //Create a VehicleData object and convert from cm/s -> m/s
    VehicleData vehicleData;
    vehicleData.setSpeed((double)(v.at(5) / 100));

    //Calculate the absolute travelled path.
    //Because we send them over 4 unsigned chars we have to put them into a
    //unsigned int (using bitshifts).
    unsigned int abstp = v.at(6) << 24;
    abstp = abstp | v.at(7) << 16;
    abstp = abstp | v.at(8) << 8;
    abstp = abstp | v.at(9);
    //Convert from cm to meters
    vehicleData.setAbsTraveledPath((abstp / 100.0));

    return (std::shared_ptr<Container>)(new Container(vehicleData));
}