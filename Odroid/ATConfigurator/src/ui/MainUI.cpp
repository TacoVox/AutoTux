//
// Created by Jonas Kahler on 4/25/16.
//

#include "ui/MainUI.h"
#include "ui/ValMonitor.h"
#include "ui/State.h"
#include "ui/CamSettings.h"
#include "ui/CamView.h"
#include <thread>

ui::MainUI::MainUI(void) : mode(0), _window(initscr()),
                           xsize(getmaxx(_window)), ysize(getmaxy(_window)),
                           interrupted(false), keyPressed((int)'\0') {
    curs_set(0);
    start_color();
    keypad(_window, true);
    nodelay(_window, true);
    refresh();

    menu = Menu(xsize, ysize);
    header = Header(xsize, ysize);
    footer = Footer(xsize, ysize);

    windows.push_back((std::unique_ptr<ATCWindow>)new ValMonitor(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new State(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamSettings(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamView(xsize, ysize));
}

void ui::MainUI::mainLoop(void) {
    while(!interrupted) {
        keyPressed = getch();

        header.refresh();
        menu.refresh();
        if(menu.getCurrent() != 4)
            windows.at(menu.getCurrent())->refresh();
        footer.refresh();

        handleKeys();

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void ui::MainUI::quit(void) {
    interrupted = true;
    endwin();
}

void ui::MainUI::handleKeys(void) {
    if(keyPressed != '\0') {
        if(keyPressed == KEY_UP && menu.isActive()) {
            menu.selUp();
        } else if(keyPressed == KEY_UP) {
            windows.at(menu.getCurrent())->selUp();
        } else if(keyPressed == KEY_DOWN && menu.isActive()) {
            menu.selDn();
        } else if(keyPressed == KEY_DOWN) {
            windows.at(menu.getCurrent())->selDn();
        } else if(keyPressed == KEY_LEFT && !menu.isActive()) {
            windows.at(menu.getCurrent())->selLeft();
        } else if(keyPressed == KEY_RIGHT && !menu.isActive()) {
            windows.at(menu.getCurrent())->selRight();
        } else if(keyPressed == KEY_RETURN) {
            menu.setActive(false);
            if(menu.getCurrent() != 4)
                windows.at(menu.getCurrent())->select();
            else
                quit();
        } else if(keyPressed == KEY_BACKSPACE || keyPressed == KEY_ESCAPE) {
            windows.at(menu.getCurrent())->unselect();
            menu.setActive(true);
        } else if(keyPressed == '+' && !menu.isActive()) {
            windows.at(menu.getCurrent())->incr();
        } else if(keyPressed == '-' && !menu.isActive()) {
            windows.at(menu.getCurrent())->decr();
        }

        keyPressed = '\0';
    }
}







