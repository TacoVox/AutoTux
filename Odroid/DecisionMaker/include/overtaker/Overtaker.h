//
// Created by marco on 2016-04-07.
//

#ifndef DECISIONMAKER_OVERTAKER_H
#define DECISIONMAKER_OVERTAKER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

using namespace automotive;
using namespace automotive::miniature;

namespace  overtaker{

    class Overtaker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    public:
        Overtaker(const int32_t &argc, char **argv);
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        virtual ~Overtaker();
        void setOvtControlDataContainer(std::shared_ptr<odcore::data::Container>);
        bool getIsOverriding();
    private:
        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;
        enum STATE {FREE_LANE, APPROACHING, LEFT_SWITCH, PARALLEL, RIGHT_SWITCH};
        enum STATE state;
        bool isOverridingControls;
        VehicleControl ovtVc;
        VehicleData vd;
        SensorBoardData sbd;
        double traveledPath;
        virtual void setUp();
        virtual void tearDown();
        std::shared_ptr<odcore::data::Container> ovtControlDataContainer;

        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double);
        void switchToLeftLane(double, double);
        void switchToRightLane(double, double);
        bool isRightLaneClear();
        double sensorValueWithLimit(const double, const double);
    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
