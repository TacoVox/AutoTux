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
    private:
        // SIMULATOR VALUES
//      const double OVT_TRIGGER = 7.5;
//      const double LEFT_SWITCH_DIST = 3.8;
//      const double ADJUST_L_S_DIST = 0.0;
//      const double RIGHT_SWITCH_DIST = 3.0;
//      const double ADJUST_R_S_DIST = 0.0;
//	    const double IR_SENSOR_RANGE = 7.0;
//	    const double US_SENSOR_RANGE = 8.0;

        //CAR VALUES
        const double OVT_TRIGGER = 0.60;
        const double LEFT_SWITCH_DIST = 0.45;
        const double ADJUST_L_S_DIST = 0.0;
        const double RIGHT_SWITCH_DIST = 0.30;
        const double ADJUST_R_S_DIST = 0.0;
	    const double IR_SENSOR_RANGE = 0.40;
	    const double US_SENSOR_RANGE = 0.40;

        // GENERAL VALUES
        const int NUM_OF_READINGS = 3;

    public:
        Overtaker();
        virtual ~Overtaker();
        void obstacleDetection(automotive::miniature::SensorBoardData, automotive::VehicleData, automotive::VehicleControl);

        bool getIsOverriding();
        bool isLeftLane();
        VehicleControl getOvtControl();

    private:
        VehicleControl ovtControl;
        bool isOverridingControls;
        double traveledPath;
        enum STATE {FREE_LANE, LEFT_SWITCH, ADJUST_LEFT_SWITCH, LEFT_LANE, PARALLEL, RIGHT_SWITCH, ADJUST_RIGHT_SWITCH};
        enum STATE state;
        bool leftLane;
        int consecReadings;
        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;

        bool isObstacleOnLane(automotive::miniature::SensorBoardData, const double);
        void switchToLeftLane(automotive::VehicleData, const double, const double);
        void adjustLeftSwitch(automotive::VehicleData, const double, const double);
        bool isParallelToObstacle(automotive::miniature::SensorBoardData);
        bool isRightLaneClear(automotive::miniature::SensorBoardData);
        void switchToRightLane(automotive::VehicleData, const double, const double);
        void adjustRightSwitch(automotive::VehicleData, const double, const double);
        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double, const double);

    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
