#include <iostream>

#include "Overtake.h"

int32_t main(int32_t argc, char **argv){
    over::take::Overtake ovt(argc, argv);
    return ovt.runModule();
}

