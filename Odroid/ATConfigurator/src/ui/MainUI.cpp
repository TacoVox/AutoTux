//
// Created by Jonas Kahler on 4/25/16.
//

#include "ui/MainUI.h"

ui::MainUI::MainUI(void) : _window(initscr()),
                           xsize(getmaxx(_window)),
                           ysize(getmaxy(_window)),
                           interrupted(false),
                           header((std::unique_ptr<Header>)new Header(xsize, ysize)),
                           footer(Footer(xsize, ysize)) {
    curs_set(0);
    start_color();
}

void ui::MainUI::showMainUI(void) {
    refresh();
    wrefresh(header->getHeader());
    wrefresh(footer.getFooter());
}

void ui::MainUI::inputLoop(void) {
    while(!interrupted) {
        std::getchar();
    }
}

void ui::MainUI::quit(void) {
     endwin();
}






