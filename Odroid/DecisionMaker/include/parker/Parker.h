//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_PARKER_H
#define DECISIONMAKER_PARKER_H

#include "opendavinci/odcore/data/Container.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace parker {
    using namespace std;

    class Parker {

    private:

        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;

        double carPosition;

        automotive::VehicleControl vc;

        void findObject(automotive::miniature::SensorBoardData);
        void findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapEnd(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void enoughSpace();
        automotive::VehicleControl adjustBeforeParking(automotive::VehicleData, int);
        automotive::VehicleControl backAroundCorner(automotive::VehicleData, int);
        automotive::VehicleControl backingStraight(automotive::VehicleData, int);
        automotive::VehicleControl backingLeft(automotive::VehicleData, int);
        automotive::VehicleControl adjustInSpotForward(automotive::VehicleData, int);
        automotive::VehicleControl adjustInSpotBack(automotive::VehicleData, int);

        bool isNotSafe(automotive::miniature::SensorBoardData);
        automotive::VehicleControl goBackToLane(automotive::VehicleData);

    public:
        Parker();
        virtual ~Parker();
        void findSpot(automotive::miniature::SensorBoardData, automotive::VehicleData);
        bool getFoundSpot();
        bool getIsParked();

        automotive::VehicleControl parallelPark(automotive::miniature::SensorBoardData, automotive::VehicleData);
    };
}
#endif //DECISIONMAKER_PARKER_H
