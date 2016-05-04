//
// Created by Jonas Kahler on 4/27/16.
//

#include "ui/Menu.h"
#include "ui/ValMonitor.h"
#include "ui/CamSettings.h"
#include "ui/CamView.h"

ui::Menu::Menu(void) { Menu(80, 20); }

ui::Menu::Menu(int x, int y) : xsize(x), ysize(y),
                               _menu(newwin(ysize - 2, 15, 1, 0)),
                               items({"Cockpit", "Camera Setup",
                                     "Camera View", "Quit"}),
                               curritem(0), currwindow(0), menusel(true) {
    windows.push_back((std::unique_ptr<ATCWindow>)new ValMonitor(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamSettings(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamView(xsize, ysize));
    wborder(_menu, ' ', ACS_VLINE, ' ', ' ', ' ', ACS_VLINE, ' ', ACS_VLINE);
}

void ui::Menu::refresh(void) {
    genMenu();
    windows.at(currwindow)->refresh();
    wrefresh(_menu);
}

void ui::Menu::selDn(void) {
    if(!menusel)
        windows.at(currwindow)->selDn();
    else if(curritem + 1 == (int)items.size())
        curritem = 0;
    else
        curritem++;
}

void ui::Menu::selUp(void) {
    if(!menusel)
        windows.at(currwindow)->selUp();
    else if(curritem == 0)
        curritem = (int)items.size() - 1;
    else
        curritem--;
}

void ui::Menu::selLeft() {
    if(!menusel)
        windows.at(currwindow)->selLeft();
}

void ui::Menu::selRight() {
    if(!menusel)
        windows.at(currwindow)->selRight();
}

void ui::Menu::select(void) {
    if (curritem == 3)
        exit(0);
    else
        currwindow = curritem;

    menusel = false;
    windows.at(currwindow)->select();
}

void ui::Menu::unselect(void) {
    windows.at(currwindow)->unselect();
    menusel = true;
}

void ui::Menu::incr() {
    if(!menusel)
        windows.at(currwindow)->incr();
}

void ui::Menu::decr() {
    if(!menusel)
        windows.at(currwindow)->decr();
}

void ui::Menu::genMenu(void) {
    int i;
    for(i = 0; i < (int)items.size(); i++) {
        if(i == curritem && menusel)
            wattron(_menu, A_STANDOUT);
        else
            wattroff(_menu, A_STANDOUT);

        if(i + 1 == (int)items.size())
            mvwaddstr(_menu, ysize - 4, 1, items.at(i).c_str());
        else
            mvwaddstr(_menu, i + 1, 1, items.at(i).c_str());
    }
}



