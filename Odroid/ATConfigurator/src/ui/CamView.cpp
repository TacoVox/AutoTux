//
// Created by jonas on 5/3/16.
//

#include "ui/CamView.h"

ui::CamView::CamView(void) { CamView(80, 20); }

ui::CamView::CamView(int x, int y) : xsize(x - 2), ysize(y - 2),
                                             _camview(newwin(ysize - 2, xsize - 16, 1, 16)) {
    mvwaddstr(_camview, 1, 1, "Cam View Test");
}

void ui::CamView::refresh(void) {
    //printVals();
    wrefresh(_camview);
}

void ui::CamView::selUp(void) {}
void ui::CamView::selDn(void) {}
void ui::CamView::selLeft(void) {}
void ui::CamView::selRight(void) {}
void ui::CamView::incr(void) {}
void ui::CamView::decr(void) {}