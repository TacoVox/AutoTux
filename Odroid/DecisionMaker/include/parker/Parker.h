//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_PARKER_H
#define DECISIONMAKER_PARKER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace parker{
    using namespace std;

    class Parker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    private:
        Parker(const Parker &/*obj*/);
        Parker &operator=(const Parker &/*obj*/);
        std::shared_ptr<bool> parking;
        std::shared_ptr<odcore::data::Container> parkingControler;
        double findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);

    public:
        Parker(const int32_t &argc, char **argv);

        virtual ~Parker();

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        bool getFoundSpot();
        void setParking(std::shared_ptr<bool>);
        void setParkingControler(std::shared_ptr<odcore::data::Container>);

    private:
        virtual void setUp();
        virtual void tearDown();
    };
} //parker
#endif //DECISIONMAKER_PARKER_H
