//
// Created by marco on 2016-04-07.
//

#ifndef DECISIONMAKER_OVERTAKER_H
#define DECISIONMAKER_OVERTAKER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace  overtaker{

    using namespace automotive;                     // Allows 'Vehicle Data' and 'Vehicle Control'

    class Overtaker {
    public:
        Overtaker();
        virtual ~Overtaker();
        void obstacleDetection(automotive::miniature::SensorBoardData, automotive::VehicleData, automotive::VehicleControl);

        bool getIsOverriding();
        VehicleControl getOvtControl();

    private:
        VehicleControl ovtControl;
        bool isOverridingControls;
        double traveledPath;
        enum STATE {FREE_LANE, APPROACHING, LEFT_SWITCH, LEFT_SWITCH_LT, LEFT_SWITCH_RT, LEFT_LANE, PARALLEL, RIGHT_SWITCH};
        enum STATE state;
        double enterSwitchAngle;

        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;

        bool isObstacleOnLane(automotive::miniature::SensorBoardData, const double);
        void switchToLeftLane(automotive::VehicleData, automotive::VehicleControl, const double, const double);
        bool isParallelToObstacle(automotive::miniature::SensorBoardData);
        bool isRightLaneClear(automotive::miniature::SensorBoardData);
        void switchToRightLane(automotive::VehicleData, automotive::VehicleControl, const double, const double);
        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double, const double);

    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
