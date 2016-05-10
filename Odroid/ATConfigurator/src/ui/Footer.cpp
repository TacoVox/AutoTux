//
// Created by jonas on 4/26/16.
//

#include "ui/Footer.h"

ui::Footer::Footer(void) : xsize(80), ysize(20),
                           _footer(newwin(1, xsize, ysize - 1, 0)),
                           time(std::time(nullptr)) {}

ui::Footer::Footer(int x, int y) : xsize(x), ysize(y),
                                   _footer(newwin(1, xsize, ysize - 1, 0)),
                                   time(std::time(nullptr)) {
    init_pair(1, 7, 4);
    wbkgd(_footer, COLOR_PAIR(1));
}

void ui::Footer::refresh(void) {
    wmove(_footer, 0, 0);
    wclrtobot(_footer);
    time = std::time(nullptr);
    std::string timestring = std::asctime(std::localtime(&time));
    mvwaddstr(_footer, 0, (xsize - (int)timestring.size()), timestring.c_str());
    wrefresh(_footer);
}

