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
        const double SPOT_SIZE = 0.70;
        const double ADJUST_BEFORE_PARKING = 0.03;
        const double BACK_AROUND_CORNER = 0.50;
        const double BACKING_STRAIGHT = 0.04;
        const double BACKING_LEFT = 0.53;
        const double ADJUST_IN_SPOT_FORWARD = 0.10;
        const double ADJUST_IN_SPOT_BACK = 0.10;

        const double SENSOR_SAFETY_MIN = 0.03;
        const double SENSOR_SAFETY_MAX = 0.04;

        const double IRSENSOR_DISTANCE_MIN = 0.02;
        const double IRSENSOR_DISTANCE_MAX = 0.20;

        const double ULTRASENSOR_DISTANCE_MIN = 0.02;
        const double ULTRASENSOR_DISTANCE_MAX = 90;
        const double ENOUGH_SPACE_DISTANCE = 0.85;

        const int SENSOR_CONVERSION = 100;
        const double DISTANCE_FROM_BACK_OBJECT = 0.10;

        //-----------------------------------------

        //SIMULATION
//        const double SPOT_SIZE = 7;
//        const double ADJUST_BEFORE_PARKING = 2;
//        const double BACK_AROUND_CORNER = 5;
//        const double BACKING_STRAIGHT = 2;
//        const double BACKING_LEFT = 2.5;
//        const double ADJUST_IN_SPOT_FORWARD = 1.5;
//        const double ADJUST_IN_SPOT_BACK = 1;
//
//        const double SENSOR_SAFETY_MIN = 0.2;
//        const double SENSOR_SAFETY_MAX = 0.5;
//        const double IRSENSOR_DISTANCE_MIN = 0;
//        const double IRSENSOR DISTANCE_MAX = 25;
//        const double ULTRSENSOR_DISTANCE_MIN = 0;
//        const double ULTRASENSOR_DISTANCE_MAX = 40;
//        const double ENOUGH_SPACE_DISTANCE = 7;
//        const int SENSOR_CONVERSION = 1;
//        const double DISTANCE_FROM_BACK_OBJECT = 2;
        //----------------------------------------


        enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, ENOUGHSPACE};
        enum PARKSTATE {PHASE0,PHASE1, PHASE2, PHASE3, PHASE4, PHASE5, PHASE6, SAFETYSTOP};
        PARKSTATE parkstate;
        STATE state;

        double carPosition;
        automotive::VehicleControl vc;
        double gapStart;
        double gapEnd;
        bool isSpot;
        bool isParked;
        int isAccurate;
        automotive::VehicleControl controlTemp;
        bool objInFront;
        int turningCounter;
        bool reversing;
        odcore::base::LIFOQueue lifoQueue;

        const int FREQUENCY = 5;

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
        automotive::VehicleControl backingLeft(automotive::VehicleData, double);
        automotive::VehicleControl adjustInSpotForward(automotive::VehicleData, double);
        automotive::VehicleControl adjustInSpotBack(automotive::VehicleData, double);
        automotive::VehicleControl midOfSpot(automotive::miniature::SensorBoardData);
        bool isNotSafe(automotive::miniature::SensorBoardData);
        automotive::VehicleControl goBackToLane(automotive::VehicleData);
        void inBetweenObjects(automotive::miniature::SensorBoardData);
        void objectBehind(automotive::miniature::SensorBoardData);

    public:
        Parker();
        virtual ~Parker();
        void findSpot(automotive::miniature::SensorBoardData, automotive::VehicleData, automotive::VehicleControl);
        bool getFoundSpot();
        bool getIsParked();
        bool isReversing();

        automotive::VehicleControl parallelPark(automotive::miniature::SensorBoardData, automotive::VehicleData);
    };
}
#endif //DECISIONMAKER_PARKER_H
