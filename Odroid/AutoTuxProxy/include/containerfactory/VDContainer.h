//
// Created by Jonas Kahler on 4/15/16.
//
#include <opendavinci/odcore/data/Container.h>
#include <vector>
#include <memory>

#ifndef AUTOTUXPROXY_VDCONTAINER_H
#define AUTOTUXPROXY_VDCONTAINER_H


namespace containerfactory {
    class VDContainer {
    public:
        static VDContainer* instance(void);
        std::shared_ptr<odcore::data::Container>
            genVDContainer(std::vector<unsigned char>);
    private:
        VDContainer();
        VDContainer(const VDContainer&);
        ~VDContainer();
        static VDContainer* _instance;
    };
}


#endif //AUTOTUXPROXY_VDCONTAINER_H
