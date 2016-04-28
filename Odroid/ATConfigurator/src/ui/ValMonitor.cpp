//
// Created by jonas on 4/26/16.
//

#include "ui/ValMonitor.h"

ui::ValMonitor::ValMonitor(void) { ValMonitor(80, 20); }

ui::ValMonitor::ValMonitor(int x, int y) : xsize(x - 2), ysize(y - 2),
                                       _valmonitor(newwin(ysize - 2, xsize - 16,
                                                          1, 16)) {
    mvwaddstr(_valmonitor, 1, 1, "Val Monitor Test");
}

void ui::ValMonitor::refresh(void) {
    wrefresh(_valmonitor);
}