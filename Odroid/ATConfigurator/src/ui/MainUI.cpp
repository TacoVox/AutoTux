//
// Created by Jonas Kahler on 4/25/16.
//

#include "ui/MainUI.h"
#include <thread>

ui::MainUI::MainUI(void) : mode(0), _window(initscr()),
                           xsize(getmaxx(_window)), ysize(getmaxy(_window)),
                           interrupted(false), keyPressed((int)'\0') {
    curs_set(0);
    start_color();
    refresh();
    menu = Menu(xsize, ysize);
    header = Header(xsize, ysize);
    footer = Footer(xsize, ysize);
}

void ui::MainUI::mainLoop(void) {
    while(!interrupted) {
        handleKeys();
        menu.refresh();
        footer.refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void ui::MainUI::inputLoop(void) {
    while(!interrupted) {
        keyPressed = getchar();
    }
}

void ui::MainUI::quit(void) {
     endwin();
}

void ui::MainUI::handleKeys(void) {
    if(keyPressed != '\0') {
        if(keyPressed == 'w')
            menu.selUp();
        else if(keyPressed == 's')
            menu.selDn();
        else if(keyPressed == 'a')
            menu.selLeft();
        else if(keyPressed == 'd')
            menu.selRight();
        else if(keyPressed == '#')
            menu.select();

        keyPressed = '\0';
    }
}







