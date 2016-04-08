//
// Created by Jonas Kahler on 4/4/16.
//
#include <opendavinci/odcore/data/Container.h>
#include <vector>
#include <memory>

#ifndef SERIALCONNECTOR_SBDCONTAINER_H
#define SERIALCONNECTOR_SBDCONTAINER_H

namespace containerfactory {
    class SBDContainer {
    public:
        static SBDContainer* instance(void);
        std::shared_ptr<odcore::data::Container>
            genSBDContainer(std::vector<unsigned char>);
    private:
        SBDContainer();
        SBDContainer(const SBDContainer&);
        ~SBDContainer();      
        static SBDContainer* _instance;
    };
}

#endif //SERIALCONNECTOR_SBDCONTAINER_H
