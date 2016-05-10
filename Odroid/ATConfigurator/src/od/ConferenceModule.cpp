//
// Created by Jonas Kahler on 4/28/16.
//

#include "od/ConferenceModule.h"

#include "od/ConferenceData.h"
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>
#include <automotivedata/generated/automotive/VehicleData.h>
#include <automotivedata/generated/automotive/VehicleControl.h>
#include <automotivedata/generated/automotive/miniature/SensorBoardData.h>
#include <automotivedata/generated/autotux/config/LaneFollowerMSG.h>
#include <automotivedata/generated/autotux/LaneRecommendationMSG.h>

using namespace std;
using namespace odcore::data;
using namespace odcore::data::image;
using namespace odcore::base::module;
using namespace odcore::wrapper;
using namespace automotive;
using namespace automotive::miniature;
using namespace autotux;
using namespace autotux::config;

od::ConferenceModule::ConferenceModule(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Configurator") {}

od::ConferenceModule::~ConferenceModule() {}

void od::ConferenceModule::setUp() {}

void od::ConferenceModule::tearDown() {}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode od::ConferenceModule::body() {
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
            odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        //Get the current VehicleControl data and put them in the storage
        Container vcc = getKeyValueDataStore().get(VehicleControl::ID());
        VehicleControl vehicleControl = vcc.getData<VehicleControl>();
        ConferenceData::instance()->setSpeed(vehicleControl.getSpeed());
        ConferenceData::instance()->setAngle(vehicleControl.getSteeringWheelAngle());

        //Get the current sensor board data
        Container sbc = getKeyValueDataStore().get(SensorBoardData::ID());
        SensorBoardData sensorBoardData = sbc.getData<SensorBoardData>();
        map<uint32_t, double> sensorMap = sensorBoardData.getMapOfDistances();
        ConferenceData::instance()->setUS1(sensorMap[3]);
        ConferenceData::instance()->setUS2(sensorMap[4]);
        ConferenceData::instance()->setIR1(sensorMap[0]);
        ConferenceData::instance()->setIR2(sensorMap[2]);
        ConferenceData::instance()->setIR3(sensorMap[1]);

        //Get the current LaneRecommendation
        Container lrc = getKeyValueDataStore().get(LaneRecommendationMSG::ID());
        LaneRecommendationMSG laneRecommendationMSG = lrc.getData<LaneRecommendationMSG>();
        ConferenceData::instance()->setRec_angle(laneRecommendationMSG.getAngle());
        ConferenceData::instance()->setDistance_to_line(laneRecommendationMSG.getDistance_to_line());
        ConferenceData::instance()->setQuality(laneRecommendationMSG.getQuality());

        //Get the current Vehicle Data
        Container vdc = getKeyValueDataStore().get(VehicleData::ID());
        VehicleData vehicleData = vdc.getData<VehicleData>();
        ConferenceData::instance()->setAbsPath(vehicleData.getAbsTraveledPath());

        //Get the shared image address
        Container image_container = getKeyValueDataStore().get(SharedImage::ID());
        if (image_container.getDataType() == SharedImage::ID())
            readSharedImage(image_container);

        //Send out the configured vals
        getConference().send(*od::ConferenceData::instance()->genLaneFollowerContainer());

        //Send state
        getConference().send(*od::ConferenceData::instance()->genDecisionMakerContainer());
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void od::ConferenceModule::readSharedImage(Container &c) {
    SharedImage si = c.getData<SharedImage>();
    if (si.getName() == "WebCam") {
        if (!m_hasAttachedToSharedImageMemory) {
            m_sharedImageMemory = SharedMemoryFactory::attachToSharedMemory(si.getName());

            m_hasAttachedToSharedImageMemory = true;
        }

        // Did we successfully connect?
        if (m_sharedImageMemory->isValid()) {
            m_sharedImageMemory->lock();
            // Create image(cv::Mat) if empty.
            if (m_image.empty()) {
                m_image.create(si.getHeight(), si.getWidth(), CV_8UC3);
            } else {
                // Copy image data form SharedImageMemory
                memcpy(m_image.data, m_sharedImageMemory->getSharedMemory(),
                       si.getHeight() * si.getWidth() * si.getBytesPerPixel());
            }
            m_sharedImageMemory->unlock();
        }

        cv::imwrite("camview.jpg", m_image);
    }
}
