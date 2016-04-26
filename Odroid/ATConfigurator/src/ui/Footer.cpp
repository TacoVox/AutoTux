//
// Created by jonas on 4/26/16.
//

#include "ui/Footer.h"

ui::Footer::Footer(int x, int y) : xsize(x), ysize(y),
                                   _footer(newwin(1, xsize, ysize - 1, 0)) {
    init_pair(1, 7, 4);
    wbkgd(_footer, COLOR_PAIR(1));
    mvwaddstr(_footer, 0, 0, "Test");
}

WINDOW *ui::Footer::getFooter(void) {
    return _footer;
}

void ui::Footer::dateLoop(void) {

}