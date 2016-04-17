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
        void setOvtControlDataContainer(std::shared_ptr<odcore::data::Container>);
        bool getIsOverriding();
        void obstacleDetection(automotive::miniature::SensorBoardData, automotive::VehicleData);
    private:
        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;

        enum STATE {FREE_LANE, APPROACHING, LEFT_SWITCH, PARALLEL, RIGHT_SWITCH};
        enum STATE state;

        bool isOverridingControls;
        VehicleControl ovtVc;
        std::shared_ptr<odcore::data::Container> ovtControlDataContainer;

        double traveledPath;
        double getTraveledPath(automotive::VehicleData);

        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double);
        bool switchToLeftLane(automotive::VehicleData, double, double);
        void switchToRightLane(automotive::VehicleData, double, double);
        bool isRightLaneClear();
        double sensorValueWithLimit(automotive::miniature::SensorBoardData, const double, const double);
    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
