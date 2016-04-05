//
// Created by Jonas Kahler on 4/4/16.
//

#include "containerfactory/SBDContainer.h"

#include <map>

#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>

using namespace std;
using namespace odcore::data;
using namespace automotive::miniature;

Container containerfactory::SBDContainer::genSBDContainer(double val) {
    map<uint32_t, double> values;
    values[1]=val;
    SensorBoardData sensorBoardData;
    sensorBoardData.setNumberOfSensors(1);
    sensorBoardData.setMapOfDistances(values);
    Container c(sensorBoardData);
    return c;
}