//
// Created by Jonas Kahler on 4/15/16.
//

#ifndef AUTOTUXPROXY_VDCONTAINER_H
#define AUTOTUXPROXY_VDCONTAINER_H


namespace containerfactory {
    class VDContainer {
    public:
        static VDContainer* instance(void);
    private:
        VDContainer();
        VDContainer(const VDContainer&);
        ~VDContainer();
        static VDContainer* _instance;
    };
}


#endif //AUTOTUXPROXY_VDCONTAINER_H
