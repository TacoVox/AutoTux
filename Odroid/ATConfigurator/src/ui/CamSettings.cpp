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

    mvwaddstr(_camsettings, 1, 1,
              dtostr("Speed", od::ConferenceData::instance()->getSpeed()));
}

const char *ui::CamSettings::dtostr(std::string field, double val) {
    std::string str = field.append(": ");
    std::ostringstream valcon;
    valcon << val;
    str.append(valcon.str());
    return str.c_str();
}



