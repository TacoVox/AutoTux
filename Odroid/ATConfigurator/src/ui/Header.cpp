/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#include "ui/Header.h"
#include <string>

ui::Header::Header() : xsize(80), ysize(20),
                       _header(newwin(1, xsize, 0, 0)) {}

ui::Header::Header(int x, int y) : xsize(x), ysize(y),
                                   _header(newwin(1, xsize, 0, 0)) {
    init_pair(1, 7, 4);
    wbkgd(_header, COLOR_PAIR(1));
}

void ui::Header::refresh(void) {
    //Clear the screen
    wmove(_header, 0, 0);
    wclrtobot(_header);
    std::string title = "AutoTux Configurator - v1.0";
    int startpos = (xsize - (int)title.size()) / 2;
    mvwaddstr(_header, 0, startpos, title.c_str());
    wrefresh(_header);
}