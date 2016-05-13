/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Singelton which acts as an overall data storage for different data fields
 */

#ifndef ATCONFIGURATOR_CONFERENCEDATA_H
#define ATCONFIGURATOR_CONFERENCEDATA_H

#include <vector>
#include <memory>
#include <tiff.h>
#include <opendavinci/odcore/data/Container.h>

namespace od {
    class ConferenceData {
    public:
        enum STATE {LANE_FOLLOWING, DRIVING, PARKING, RESUME};

        // Method returning a pointer to the Singelton instance
        static ConferenceData* instance(void);

        /**
        * Method to create a shared pointer to a container wrapping a LaneFollowerMSG
        * containing the setup values for the camera.
        */
        std::shared_ptr<odcore::data::Container> genLaneFollowerContainer(void);

        /**
        * Method to create a pointer to a container around a DecisionMakerMSG. This
        * message contains the state selected in the GUI.
        */
        std::shared_ptr<odcore::data::Container> genDecisionMakerContainer(void);

        /**
         * Different getters and setters for the class fields
         */
        bool isCamView(void);
        void setCamView(bool);
        double getSpeed(void);
        void setSpeed(double);
        double getAngle(void);
        void setAngle(double);
        double getUS1(void);
        void setUS1(double);
        double getUS2(void);
        void setUS2(double);
        double getIR1(void);
        void setIR1(double);
        double getIR2(void);
        void setIR2(double);
        double getIR3(void);
        void setIR3(double);
        double getRec_angle(void);
        void setRec_angle(double);
        double getDistance_to_line(void);
        void setDistance_to_line(double);
        bool isQuality(void);
        void setQuality(bool);
        double getAbsPath(void);
        void setAbsPath(double);
        uint32 getRoadWidth(void);
        void setRoadWidth(uint32);
        uint8 getRoadOffset(void);
        void setRoadOffset(uint8);
        double getGainP(void);
        void setGainP(double);
        double getGainI(void);
        void setGainI(double);
        double getGainD(void);
        void setGainD(double);
        uint8 getThresholdB(void);
        void setThresholdB(uint8);
        uint8 getThresholdD(void);
        void setThresholdD(uint8);
        STATE getState(void);
        void setState(STATE);
    private:
        /**
         * Private constructors and destructor to ensure that just the singelton
         * itself creates instances
        */
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();

        // Different private fields.
        bool camView;
        double speed;
        double angle;
        double us1;
        double us2;
        double ir1;
        double ir2;
        double ir3;
        double rec_angle;
        double distance_to_line;
        bool quality;
        double absPath;
        uint32 roadWidth;
        uint8 roadOffset;
        double gainP;
        double gainI;
        double gainD;
        uint8 thresholdB;
        uint8 thresholdD;
        STATE state;

        // Pointer to the actual instance
        static ConferenceData* _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
