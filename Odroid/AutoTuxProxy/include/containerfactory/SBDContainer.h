//
// Created by Jonas Kahler on 4/4/16.
//
#include <opendavinci/odcore/data/Container.h>

#ifndef SERIALCONNECTOR_SBDCONTAINER_H
#define SERIALCONNECTOR_SBDCONTAINER_H

namespace containerfactory {
    class SBDContainer {
    public:
        odcore::data::Container genSBDContainer(double);
    };
}

#endif //SERIALCONNECTOR_SBDCONTAINER_H
