//
// Created by Jonas Kahler on 4/27/16.
//

#include "ui/Menu.h"

ui::Menu::Menu(void) : xsize(80), ysize(20),
                       _menu(newwin(ysize - 2, 15, 1, 0)),
                       items({"Cockpit", "Camera Setup", "Camera View",
                              "Quit"}),
                       curritem(0) { }

ui::Menu::Menu(int x, int y) : xsize(x), ysize(y),
                               _menu(newwin(ysize - 2, 15, 1, 0)),
                               items({"Cockpit", "Camera Setup",
                                     "Camera View", "Quit"}),
                               curritem(0) {
    wborder(_menu, ' ', ACS_VLINE, ' ', ' ', ' ', ACS_VLINE, ' ', ACS_VLINE);
}

void ui::Menu::refresh(void) {
    genMenu();
    wrefresh(_menu);
}

void ui::Menu::genMenu(void) {
    int i;
    for(i = 0; i < (int)items.size(); i++) {
        if(i == curritem)
            wattron(_menu, A_STANDOUT);
        else
            wattroff(_menu, A_STANDOUT);

        if(i + 1 == (int)items.size())
            mvwaddstr(_menu, ysize - 4, 1, items.at(i).c_str());
        else
            mvwaddstr(_menu, i + 1, 1, items.at(i).c_str());
    }
}

void ui::Menu::selDown(void) {
    if(curritem != (int)items.size())
        curritem++;
    else
        curritem = 0;
}

void ui::Menu::selUp(void) {
    if(curritem != 0)
        curritem--;
    else
        curritem = (int)items.size();
}