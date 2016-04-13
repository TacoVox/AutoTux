//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "parker/Parker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace parker;

bool foundSpot;

Parker::Parker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Parker") {}

Parker::~Parker() {}

void Parker::setUp(){
    cout << "Parker starts" << endl;
    foundSpot = false;
}
void Parker::tearDown(){
    cout << "This is when Parker stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parker::body() {

    const double INFRARED_REAR_RIGHT = 2; //Sensor

    enum STATE {FINDGAPSTART, FINDGAPEND, ADJUST, PARK};

    bool objectFound = false;
    double gapStart = 0;
    double gapEnd = 0;

    STATE state;


    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        double absTraveled = 0;
        double parkPosition = 0;
        int parkState = 0;
        /**
         * @TODO Need to take these way later because it shall take the speed of the decisionmaker
         */
        VehicleControl vc;


        while(*parking){
            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData>();

            // 2. Get most recent sensor board data describing virtual sensor data:
            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData>();

            // Create vehicle control data.


            if(objectFound){
                absTraveled = vd.getAbsTraveledPath();

                if((state == FINDGAPSTART) && (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 0)){
                    gapStart = vd.getAbsTraveledPath();
                    state = FINDGAPEND;
                    cout << "Found gap START" << endl;
                }
                if((state == FINDGAPEND) && (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0)){
                    cout << "Found gap END" << endl;
                    gapEnd = vd.getAbsTraveledPath();
                    cout << "This is the gap size: " << (gapEnd - gapStart) << endl;
                    if((gapEnd - gapStart) > 7){
                        cout << "spot has been found----------------" << endl;
                        state = ADJUST;
                        foundSpot = true;
                    }
                    else{
                        cout << "IT WASN'T BIG ENOUGH" << endl;
                        objectFound = false;
                    }
                }
                if(state == ADJUST){
                    if(gapEnd + 2 > absTraveled){
                        vc.setSpeed(.6);
                        vc.setSteeringWheelAngle(0);
                        *parkingControler = vc;
                    }
                    if(gapEnd + 2 < absTraveled){
                        vc.setSpeed(0);
                        *parkingControler = vc;
                        state = PARK;
                        parkPosition = absTraveled;
                    }
                }
                /**
                 * @TODO Fix so that there is a safty check so that the car dosn't drive in to the back car
                 */

                if(state == PARK){

                    switch (parkState){
                        // Make the car go back and turn around the corner of the front car
                        case 0:{
                            cout << "Current speed: " << vd.getSpeed() << endl;
                            if(parkPosition + 5 > absTraveled){
                                vc.setSpeed(-0.6);
                                vc.setSteeringWheelAngle(45);
                                *parkingControler = vc;
                                break;
                            }
                                // Prepare for next step with straight wheels and change case
                            else if(parkPosition + 5 < absTraveled){
                                vc.setSpeed(0);
                                vc.setSteeringWheelAngle(0);
                                *parkingControler = vc;
                                parkState = 1;
                                break;
                            }
                            break;
                        }
                        // Goes straight back and after the distance turn the wheels -45 to make the car straight
                        case 1:{
                            cout << "inside case 1: " << endl;

                            if(parkPosition + 7 > absTraveled){
                                vc.setSpeed(-0.5);
                                *parkingControler = vc;
                                break;
                            }
                            else if(parkPosition + 7 < absTraveled){
                                vc.setSteeringWheelAngle(-45);
                                *parkingControler = vc;
                                parkState = 2;
                                break;
                            }
                            break;
                        }
                        //Continues to go backwards until the given length
                        case 2:{
                            if(parkPosition + 9.5 < absTraveled){
                                vc.setSpeed(0);
                                *parkingControler = vc;
                                parkState = 3;
                                break;
                            }
                            else break;
                        }
                        //Adjusts the car to go forward to get straighter
                        case 3:{
                            if(parkPosition + 11 > absTraveled){
                                vc.setSpeed(0.3);
                                vc.setSteeringWheelAngle(30);
                                *parkingControler = vc;
                                break;
                            }
                            else if(parkPosition + 11 < absTraveled){
                                vc.setSpeed(0);
                                vc.setSteeringWheelAngle(30);
                                *parkingControler = vc;
                                parkState = 4;
                                break;
                            }
                            break;
                        }
                        //Adjusts by going backwards
                        case 4:{
                            if(parkPosition + 11.5 > absTraveled){
                                vc.setSpeed(-0.3);
                                vc.setSteeringWheelAngle(-15);
                                *parkingControler = vc;
                            }
                            else if(parkPosition + 11.5 < absTraveled){
                                vc.setSpeed(0);
                                *parkingControler = vc;
                                objectFound = false;
                            }
                        }
                    }
                }
            }
                //If no states are set yet for the Parking and it finds an object
            else if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0){
                cout << "object found" << endl;
                objectFound = true;
                state = FINDGAPSTART;
            }
        }
    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
/**
 * This method sets the bool parking passed from DecisionMaker
 */
void Parker::setParking(std::shared_ptr<bool> parking){
    this->parking = parking;
}
void Parker::setParkingControler(std::shared_ptr<Container> parkingControler){
    this->parkingControler = parkingControler;
}
bool Parker::getFoundSpot(){
    if(foundSpot)
        return true;
    else
        return false;
}


