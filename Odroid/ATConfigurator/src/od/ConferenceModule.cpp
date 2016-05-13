/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

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
        TimeTriggeredConferenceClientModule(argc, argv, "Configurator"),
        m_hasAttachedToSharedImageMemory(false), m_sharedImageMemory(), m_image() {}

od::ConferenceModule::~ConferenceModule() {}

void od::ConferenceModule::setUp() {}
void od::ConferenceModule::tearDown() {}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode od::ConferenceModule::body() {
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
            odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        // Get the current VehicleControl data and put them in the storage
        Container vcc = getKeyValueDataStore().get(VehicleControl::ID());
        VehicleControl vehicleControl = vcc.getData<VehicleControl>();
        // Set the according fields in our ConferenceData singleton
        ConferenceData::instance()->setSpeed(vehicleControl.getSpeed());
        ConferenceData::instance()->setAngle(vehicleControl.getSteeringWheelAngle());

        // Get the current sensor board data
        Container sbc = getKeyValueDataStore().get(SensorBoardData::ID());
        SensorBoardData sensorBoardData = sbc.getData<SensorBoardData>();
        map<uint32_t, double> sensorMap = sensorBoardData.getMapOfDistances();
        // Set the according fields in our ConferenceData singleton
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

        // Get the current Vehicle Data
        Container vdc = getKeyValueDataStore().get(VehicleData::ID());
        VehicleData vehicleData = vdc.getData<VehicleData>();
        // Set the according fields in our ConferenceData singleton
        ConferenceData::instance()->setAbsPath(vehicleData.getAbsTraveledPath());

        // Get the shared image address
        Container image_container = getKeyValueDataStore().get(SharedImage::ID());
        // Just IF the CamView is selected we will fetch store the image on the
        // drive to save some processing power.
        if (image_container.getDataType() == SharedImage::ID() && od::ConferenceData::instance()->isCamView())
            readSharedImage(image_container);

        // Send out the configured vals
        getConference().send(*od::ConferenceData::instance()->genLaneFollowerContainer());

        // Send the selected state
        getConference().send(*od::ConferenceData::instance()->genDecisionMakerContainer());
    }

    // If everyhting went ok until that point, return the OpenDaVINCI's way of
    // saying ok. ;)
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void od::ConferenceModule::readSharedImage(Container &c) {
    SharedImage si = c.getData<SharedImage>();
    // Check if the image comes from the WebCam (the ASCII converted works best
    // with the colored image - the processed image causes problems).
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

        // Store image in the folder of execution as camview.jpg
        cv::imwrite("camview.jpg", m_image);
    }
}
