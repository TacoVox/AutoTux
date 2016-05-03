//
// Created by jonas on 4/26/16.
//

#include "ui/ValMonitor.h"

#include <sstream>
#include "od/ConferenceData.h"

ui::ValMonitor::ValMonitor(void) { ValMonitor(80, 20); }

ui::ValMonitor::ValMonitor(int x, int y) : xsize(x - 2), ysize(y - 2),
                                       _valmonitor(newwin(ysize - 2, xsize - 16,
                                                          1, 16)) {
    mvwaddstr(_valmonitor, 1, 1, "Val Monitor Test");
}

void ui::ValMonitor::refresh(void) {
    printVals();
    wrefresh(_valmonitor);
}

void ui::ValMonitor::printVals(void) {
    //Clear the screen
    wmove(_valmonitor, 1, 1);
    wclrtobot(_valmonitor);

    //Current steering stuff
    mvwaddstr(_valmonitor, 1, 1,
              dtostr("Speed", od::ConferenceData::instance()->getSpeed()));
    mvwaddstr(_valmonitor, 2, 1,
              dtostr("Angle", od::ConferenceData::instance()->getAngle()));

    //Current sensor values
    mvwaddstr(_valmonitor, 1, 15,
              dtostr("US1", od::ConferenceData::instance()->getUS1()));
    mvwaddstr(_valmonitor, 2, 15,
              dtostr("US2", od::ConferenceData::instance()->getUS2()));
    mvwaddstr(_valmonitor, 3, 15,
              dtostr("IR1", od::ConferenceData::instance()->getIR1()));
    mvwaddstr(_valmonitor, 4, 15,
              dtostr("IR2", od::ConferenceData::instance()->getIR2()));
    mvwaddstr(_valmonitor, 5, 15,
              dtostr("IR3", od::ConferenceData::instance()->getIR3()));
}

const char* ui::ValMonitor::dtostr(std::string field, double val) {
    std::string str = field.append(": ");
    std::ostringstream valcon;
    valcon << val;
    str.append(valcon.str());
    return str.c_str();
}