//
// Created by marco on 2016-04-07.
//

#ifndef DECISIONMAKER_OVERTAKER_H
#define DECISIONMAKER_OVERTAKER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

using namespace automotive;

namespace  overtaker{

    class Overtaker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    public:
        Overtaker(const int32_t &argc, char **argv);
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        virtual ~Overtaker();
        void setOvtControlDataContainer(std::shared_ptr<odcore::data::Container>);
    private:
        bool isOverridingControls;
        VehicleControl ovtVc;
        virtual void setUp();
        virtual void tearDown();
        std::shared_ptr<odcore::data::Container> ovtControlDataContainer;

        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double, const double);
        void switchToLeftLane(VehicleControl);
        void switchToLeftLane();
    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
