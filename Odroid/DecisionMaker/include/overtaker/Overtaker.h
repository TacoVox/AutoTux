//
// Created by marco on 2016-04-07.
//

#ifndef DECISIONMAKER_OVERTAKER_H
#define DECISIONMAKER_OVERTAKER_H

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace  overtaker{

    using namespace automotive;                     // Allows 'Vehicle Data' and 'Vehicle Control'

    class Overtaker {
    private:
        // SIMULATOR VALUES
     	//const double OVT_TRIGGER = 7.0;
		//const double LEFT_SWITCH_DIST = 3.6;
		//const double ADJUST_L_S_DIST = 3.6;
		//const double RIGHT_SWITCH_DIST = 4.0;
		//const double ADJUST_R_S_DIST = 2.0;
		//const double US_SENSOR_RANGE = 8.0;
		//const double US_FRONT_RIGHT_RANGE = 3.0;

		//const double IR_SENSOR_LIMIT = 7.0;
		//const double US_SENSOR_LIMIT = 15.0;

		//CAR VALUES
		// *** On a straight, it needs to look further ahead, less if it's in a turn ***
		const double OVT_TRIGGER = 0.70;
		// *** On straight a straight, left switch distance needs to be longer ***
		const double LEFT_SWITCH_DIST = 0.00;
		const double ADJUST_L_S_DIST = 0.50;
		const double RIGHT_SWITCH_DIST = 0.68;
		const double ADJUST_R_S_DIST = 0.0;
		const double US_SENSOR_RANGE = 0.40;
		const double US_FRONT_RIGHT_RANGE = 0.30;

		const double IR_SENSOR_LIMIT = 0.28;
		const double US_SENSOR_LIMIT = 0.90;

		// *** GENERAL VALUES
        const int NUM_OF_READINGS = 1;

		// *** Sensors mapping
		const double ULTRASONIC_FRONT_RIGHT = 4;
		const double ULTRASONIC_FRONT_FORWARD = 3;
		const double INFRARED_FRONT_RIGHT = 0;
		const double INFRARED_REAR_RIGHT = 2;

    public:
        Overtaker();
        virtual ~Overtaker();
        void obstacleDetection(automotive::miniature::SensorBoardData, automotive::VehicleData);
		void newObstacleDetection(automotive::miniature::SensorBoardData, automotive::VehicleData);

        bool getIsOverriding();
        bool isLeftLane();
        VehicleControl getOvtControl();
		void setLaneFollowerAngle(double);

    private:
        VehicleControl ovtControl;
        bool isOverridingControls;
        bool checkReadingsCounter();

		double traveledPath, totalTraveled;
        enum STATE {FREE_LANE, INIT_LEFT_SWITCH, LEFT_SWITCH, ADJUST_LEFT_SWITCH, SEARCH_END,
					REACH_END, RIGHT_SWITCH, ADJUST_RIGHT_SWITCH};
        enum STATE state;
        bool leftLane;
        int consecReadings, idle_frame_counter;
        double min_us_fr;
		double laneFollowerAngle;

		void stopCar();
		bool isObstacleDetected(automotive::miniature::SensorBoardData, const double, const double);


        bool isObstacleOnLane(automotive::miniature::SensorBoardData, const double);
		
		bool turnLeft(automotive::VehicleData, const double, const double);
		bool cornerDetection(automotive::miniature::SensorBoardData, automotive::VehicleData, const int, const double, const double);
		bool adjustLeftSwitch(automotive::VehicleData, const double, const double);

		void keepParallelToObstacle(automotive::miniature::SensorBoardData, const double);

		bool detectEndOfObstacle(automotive::miniature::SensorBoardData);
        bool isRightLaneClear(automotive::miniature::SensorBoardData);
        bool switchToRightLane(automotive::VehicleData, const double, const double);
        bool adjustRightSwitch(automotive::VehicleData, const double, const double);
		void resetFSM(automotive::VehicleData, const double, const int);
    };
}// overtaker

#endif //DECISIONMAKER_OVERTAKER_H
