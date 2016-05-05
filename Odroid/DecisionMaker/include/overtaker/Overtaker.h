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
     	//const double OVT_TRIGGER = 7.5;
	//const double LEFT_SWITCH_DIST = 3.6;
	//const double ADJUST_L_S_DIST = 2.0;
	//const double RIGHT_SWITCH_DIST = 3.0;
	//const double ADJUST_R_S_DIST = 2.4;
	//const double IR_SENSOR_RANGE = 7.0;
	//const double US_SENSOR_RANGE = 8.0;
	//const double US_FRONT_RIGHT_RANGE = 3.0;

        //CAR VALUES

	// On a straight, it needs to look further ahead, less if it's in a turn
     const double OVT_TRIGGER = 0.60;
	// On straight a straight, left switch distance needs to be longer
    const double LEFT_SWITCH_DIST = 0.00;
    const double SWITCH_US_THRESHOLD = 0.2;
    const double ADJUST_L_S_DIST = 0.30;
    const double RIGHT_SWITCH_DIST = 0.50;
    const double ADJUST_R_S_DIST = 0.1;
    const double IR_SENSOR_RANGE = 0.28;
    const double US_SENSOR_RANGE = 0.40;
    const double US_FRONT_RIGHT_RANGE = 0.30;
    const double SENSOR_RANGE = 0.90;
        // GENERAL VALUES
        const int NUM_OF_READINGS = 1;

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
        enum STATE {FREE_LANE, LEFT_SWITCH, ADJUST_LEFT_SWITCH, LEFT_LANE, PARALLEL, RIGHT_SWITCH, ADJUST_RIGHT_SWITCH, TMP_SWITCH};
        enum STATE state;
        bool leftLane;
        int consecReadings;
        double min_us_fr;
        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;

        bool isObstacleOnLane(automotive::miniature::SensorBoardData, const double);
		bool isOnLeftLane(automotive::miniature::SensorBoardData, automotive::VehicleData, const double, const double);
		void switchToLeftLane(automotive::VehicleData, const double, const double);
        bool adjustLeftSwitch(automotive::VehicleData, const double, const double);

		bool isParallelToObstacle(automotive::miniature::SensorBoardData);
        bool isRightLaneClear(automotive::miniature::SensorBoardData);
        bool switchToRightLane(automotive::VehicleData, const double, const double);
        bool adjustRightSwitch(automotive::VehicleData, const double, const double);
        bool isObstacleDetected(automotive::miniature::SensorBoardData, const double, const double);

    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
