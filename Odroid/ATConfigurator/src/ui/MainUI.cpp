//
// Created by Jonas Kahler on 4/25/16.
//

#include "ui/MainUI.h"
#include <thread>
#include <chrono>

ui::MainUI::MainUI(void) : _window(initscr()),
                           xsize(getmaxx(_window)), ysize(getmaxy(_window)),
                           interrupted(false), header(), footer() {
    curs_set(0);
    start_color();
}

void ui::MainUI::showMainUI(void) {
    refresh();
    header = Header(xsize, ysize);
    footer = Footer(xsize, ysize);
    wrefresh(header.getHeader());
    wrefresh(footer.getFooter());
}

void ui::MainUI::inputLoop(void) {
    while(!interrupted) {
        footer.refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void ui::MainUI::quit(void) {
     endwin();
}






