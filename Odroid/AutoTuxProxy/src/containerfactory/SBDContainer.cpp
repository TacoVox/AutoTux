/*
 * Class for creating SensorBoardData Objects and wrapping them into a
 * container.
 */
#include "containerfactory/SBDContainer.h"
#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>
#include <iostream>

using namespace std;
using namespace odcore::data;
using namespace automotive::miniature;

containerfactory::SBDContainer* containerfactory::SBDContainer::_instance = 0;

//Private Constructors & Destructor so that just one element can exist.
//And this very one should be created by the class itself.
containerfactory::SBDContainer::SBDContainer() {}
containerfactory::SBDContainer::SBDContainer(const SBDContainer &) {}
containerfactory::SBDContainer::~SBDContainer() {}

//Returns an instance of that (singelton)class
containerfactory::SBDContainer* containerfactory::SBDContainer::instance(void) {
    if (!_instance)
        _instance = new containerfactory::SBDContainer();
    return _instance;
}

//Method for creating a the very Object and returning a (shared) pointer to it.
shared_ptr<Container> containerfactory::SBDContainer::genSBDContainer(vector<unsigned char> values) {
    //Create a map for all the sensor data
    map<uint32_t, double> sensordata;

    //Receiving order: (US1 byte)(US2 byte)(IR1 byte)(IR2 byte)(IR3 byte)
    //OD order: (IR1 byte)(IR3 byte)(IR2 byte)(US1 byte)(US2 byte)
    sensordata[0] = values.at(2) / 100.0;
    cout << "IR1: " << sensordata[0];
    sensordata[1] = values.at(4) / 100.0;
    cout << "IR3: " << sensordata[1];
    sensordata[2] = values.at(3) / 100.0;
    cout << "IR2: " << sensordata[2] << endl;
    sensordata[3] = values.at(0) / 100.0;
    sensordata[4] = values.at(1) / 100.0;
    sensordata[5] = values.at(10);

    //Wrap the information in a SensorBoardData object
    SensorBoardData sensorBoardData;
    sensorBoardData.setNumberOfSensors((uint32_t)values.size());
    sensorBoardData.setMapOfDistances(sensordata);

    //Wrap the newly created SensorBoardData in a Container obj and return it
    //Container c(sensorBoardData);
    //return c;
    return (std::shared_ptr<Container>)(new Container(sensorBoardData));
}
