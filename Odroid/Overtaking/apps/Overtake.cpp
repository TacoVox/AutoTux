#include <iostream>
#include <Parking.h>

#include <thread>

#include "Overtake.h"

using namespace std;

void initOvertake(int32_t, char**);

void initParking(int32_t, char**);

int32_t main(int32_t argc, char **argv){

    thread overtake (initOvertake, argc, argv);
    thread parking (initParking, argc, argv);

    overtake.join();
    parking.join();


    return 0;



}

void initOvertake(int32_t argc, char **argv){

    over::take::Overtake ovt (argc, argv);
    ovt.runModule();
}

void initParking(int32_t argc, char **argv){
    over::take::Parking prk(argc, argv);
    prk.runModule();
}

