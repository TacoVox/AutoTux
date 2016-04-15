//
// Created by Jonas Kahler on 4/15/16.
//

#include "containerfactory/VDContainer.h"

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