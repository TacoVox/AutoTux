//
// Created by Jonas Kahler on 4/4/16.
//

#include "containerfactory/SBDContainer.h"
#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>

using namespace std;
using namespace odcore::data;
using namespace automotive::miniature;

containerfactory::SBDContainer* containerfactory::SBDContainer::_instance = 0;

//Private Constructors & Destructor so that just one element can exist
containerfactory::SBDContainer::SBDContainer() {}
containerfactory::SBDContainer::SBDContainer(const SBDContainer &) {}
containerfactory::SBDContainer::~SBDContainer() {}

//Returns singelton of that class
containerfactory::SBDContainer* containerfactory::SBDContainer::instance(void) {
    if (!_instance)
        _instance = new containerfactory::SBDContainer();
    return _instance;
}

shared_ptr<Container> containerfactory::SBDContainer::genSBDContainer(vector<unsigned char> values) {
    //Create a map for all the sensor data
    map<uint32_t, double> sensordata;

    //Iterate over the vector and put the stuff in the map
    int i = 1;
    for(vector<unsigned char>::iterator it = values.begin(); it != values.end(); ++it) {
        sensordata[i] = (double)*it;
        i++;
    }

    //Wrap the information in a SensorBoardData object
    SensorBoardData sensorBoardData;
    sensorBoardData.setNumberOfSensors(values.size());
    sensorBoardData.setMapOfDistances(sensordata);

    //Wrap the newly created SensorBoardData in a Container obj and return it
    //Container c(sensorBoardData);
    //return c;
    return (std::shared_ptr<Container>)(new Container(sensorBoardData));
}
