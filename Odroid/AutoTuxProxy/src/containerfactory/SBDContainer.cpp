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

    //Receiving order: (US1 byte)(US2 byte)(IR1 byte)(IR2 byte)(IR3 byte)
    //OD order: (IR1 byte)(IR2 byte)(IR3 byte)(US1 byte)(US2 byte)
    sensordata[0] = values.at(2);
    sensordata[1] = values.at(3);
    sensordata[2] = values.at(4);
    sensordata[3] = values.at(0);
    sensordata[4] = values.at(1);

    //Wrap the information in a SensorBoardData object
    SensorBoardData sensorBoardData;
    sensorBoardData.setNumberOfSensors(values.size());
    sensorBoardData.setMapOfDistances(sensordata);

    //Wrap the newly created SensorBoardData in a Container obj and return it
    //Container c(sensorBoardData);
    //return c;
    return (std::shared_ptr<Container>)(new Container(sensorBoardData));
}
