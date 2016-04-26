//
// Created by jonas on 4/26/16.
//

#include "ui/Header.h"
#include <string>

ui::Header::Header(int x, int y) : xsize(x), ysize(y),
                                   _header(newwin(1, xsize, 0, 0)) {
    std::string title = "AutoTux Configurator - v0.1";
    init_pair(1, 7, 4);
    wbkgd(_header, COLOR_PAIR(1));
    int startpos = (xsize - (int)title.size()) / 2;
    mvwaddstr(_header, 0, startpos, title.c_str());
}

WINDOW *ui::Header::getHeader(void) {
    return _header;
}

ui::Header::~Header() {

}
