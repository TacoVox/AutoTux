#include <iostream>
#include "LaneFollower.h"

int32_t main(int32_t argc, char **argv) {
    lane::follower::LaneFollower lf(argc, argv);
    return lf.runModule();
}