//
// Created by jonas on 5/3/16.
//

#include <sstream>
#include "ui/CamSettings.h"
#include "od/ConferenceData.h"

ui::CamSettings::CamSettings(void) { CamSettings(80, 20); }

ui::CamSettings::CamSettings(int x, int y) : xsize(x - 2), ysize(y - 2),
                                           _camsettings(newwin(ysize - 2, xsize - 16, 1, 16)) {
    mvwaddstr(_camsettings, 1, 1, "Cam Settings Test");
}

void ui::CamSettings::refresh(void) {
    printVals();
    wrefresh(_camsettings);
}

void ui::CamSettings::printVals(void) {
    //Clear the screen
    wmove(_camsettings, 1, 1);
    wclrtobot(_camsettings);

    mvwaddstr(_camsettings, 1, 1, "Gain P:");
    mvwprintw(_camsettings, 1, 9, "%g", od::ConferenceData::instance()->getGainP());

    mvwaddstr(_camsettings, 2, 1, "Gain D:");
    mvwprintw(_camsettings, 2, 9, "%g", od::ConferenceData::instance()->getGainD());

    mvwaddstr(_camsettings, 3, 1, "Gain I:");
    mvwprintw(_camsettings, 3, 9, "%g", od::ConferenceData::instance()->getGainI());

    mvwaddstr(_camsettings, 1, 20, "Road Width:");
    mvwprintw(_camsettings, 1, 36, "%u", od::ConferenceData::instance()->getRoadWidth());

    mvwaddstr(_camsettings, 2, 20, "Threshold Low:");
    mvwprintw(_camsettings, 2, 36, "%u", od::ConferenceData::instance()->getThresholdD());

    mvwaddstr(_camsettings, 3, 20, "Threshold High:");
    mvwprintw(_camsettings, 3, 36, "%u", od::ConferenceData::instance()->getThresholdB());
}


