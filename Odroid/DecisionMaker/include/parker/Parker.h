//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_PARKER_H
#define DECISIONMAKER_PARKER_H



#include "opendavinci/odcore/data/Container.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/odcore/base/LIFOQueue.h"

namespace parker {
    using namespace std;

    class Parker {

    private:
        //REAL CAR
        const double SPOT_SIZE = 0.60;
        const double ADJUST_BEFORE_PARKING = 0.20;
        const double BACK_AROUND_CORNER = 0.56;
        const double BACKING_STRAIGHT = 0.00;
        const double PARALLEL_IN_SPOT = 0.66;

        const double SENSOR_SAFETY_MIN = 0.04;
        const double SENSOR_SAFETY_MAX = 0.06;

        const double IRSENSOR_DISTANCE_MIN = 0.05;
        const double IRSENSOR_DISTANCE_MAX = 0.25;

        const double SENSORS_IN_SPOT = 0.19;

        const double ULTRASENSOR_DISTANCE_MIN = 0.05;
        const double ULTRASENSOR_DISTANCE_MAX = 0.90;
        const double ENOUGH_SPACE_DISTANCE = 1.05;

        const double SENSOR_DIFFERENCE_INBETWEEN = 0.15;
        const double SENSOR_DIFFERENCE_NO_FRONT = 0.15;

        const double DISTANCE_FROM_BACK_OBJECT = 0.08;

        //-----------------------------------------
/*
        //SIMULATION
        const double SPOT_SIZE = 7;
        const double ADJUST_BEFORE_PARKING = 2;
        const double BACK_AROUND_CORNER = 5;
        const double BACKING_STRAIGHT = 2.3;
        const double PARALLEL_IN_SPOT = 6.2;

        const double SENSOR_SAFETY_MIN = 0.2;
        const double SENSOR_SAFETY_MAX = 0.5;
        const double IRSENSOR_DISTANCE_MIN = 2;
        const double IRSENSOR_DISTANCE_MAX = 25;
        const double ULTRASENSOR_DISTANCE_MIN = 2;
        const double ULTRASENSOR_DISTANCE_MAX = 40;
        const double ENOUGH_SPACE_DISTANCE = 7.5;

        const double SENSORS_IN_SPOT = 2;

        const double SENSOR_DIFFERENCE_INBETWEEN = 0.3;
        const double SENSOR_DIFFERENCE_NO_FRONT = 1.2;
        const double DISTANCE_FROM_BACK_OBJECT = 2;
        //----------------------------------------
*/

        enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, CHECKSPACE};
        enum PARKSTATE {PHASE0,PHASE1, PHASE2, PHASE3, PHASE4, SAFETYSTOP};
        PARKSTATE parkstate;
        STATE state;

        double carPosition;
        automotive::VehicleControl vc;
        double gapStart;
        double gapEnd;
        bool isSpot;
        bool isParked;
        int expectedValue;
        automotive::VehicleControl controlTemp;
        bool objInFront;
        int turningCounter;
        bool reversing;
        bool outOfSpot;
        bool isInSpot;

        const int OCCURRENCE = 3;

        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;

        void findObject(automotive::miniature::SensorBoardData);
        void findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapEnd(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void enoughSpace();
        automotive::VehicleControl adjustBeforeParking(automotive::VehicleData, double);
        automotive::VehicleControl backAroundCorner(automotive::VehicleData, double);
        automotive::VehicleControl backingStraight(automotive::VehicleData, double);
        automotive::VehicleControl getParallelInSpot(automotive::miniature::SensorBoardData, automotive::VehicleData, double);
        automotive::VehicleControl midOfSpot(automotive::miniature::SensorBoardData, automotive::VehicleData);
        bool isNotSafe(automotive::miniature::SensorBoardData);
        void inBetweenObjects(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void objectBehind(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void checkIfInSpot(automotive::miniature::SensorBoardData);
        bool sensorClear(double);
        automotive::VehicleControl stop();

    public:
        Parker();
        virtual ~Parker();
        void findSpot(automotive::miniature::SensorBoardData, automotive::VehicleData, automotive::VehicleControl);
        bool getFoundSpot();
        bool getIsParked();
        bool isReversing();
        bool isOutOfSpot();
        automotive::VehicleControl goBackToLane(automotive::miniature::SensorBoardData, automotive::VehicleData, double);
        automotive::VehicleControl parallelPark(automotive::miniature::SensorBoardData, automotive::VehicleData);
    };
}
#endif //DECISIONMAKER_PARKER_H
