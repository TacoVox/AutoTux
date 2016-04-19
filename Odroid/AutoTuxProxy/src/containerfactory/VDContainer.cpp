//
// Created by Jonas Kahler on 4/15/16.
//

#include "containerfactory/VDContainer.h"
#include <automotivedata/generated/automotive/VehicleData.h>

using namespace odcore::data;
using namespace automotive;

containerfactory::VDContainer* containerfactory::VDContainer::_instance = 0;

//Private Constructors & Destructor so that just one element can exist
containerfactory::VDContainer::VDContainer() {}
containerfactory::VDContainer::VDContainer(const VDContainer &) {}
containerfactory::VDContainer::~VDContainer() {}

//Returns singelton of that class
containerfactory::VDContainer* containerfactory::VDContainer::instance(void) {
    if (!_instance)
        _instance = new containerfactory::VDContainer();
    return _instance;
}

std::shared_ptr<Container> containerfactory::VDContainer::genVDContainer(
        std::vector<unsigned char> v) {
    VehicleData vehicleData;

    vehicleData.setSpeed((double)v.at(0) * 100);
    vehicleData.setAbsTraveledPath((double)v.at(0) * 100);

    return (std::shared_ptr<Container>)(new Container(vehicleData));
}