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
    keypad(_window, true);
    nodelay(_window, true);
    while(!interrupted) {
        keyPressed = getch();
        handleKeys();
        menu.refresh();
        footer.refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void ui::MainUI::quit(void) {
     endwin();
}

void ui::MainUI::handleKeys(void) {
    if(keyPressed != '\0') {
        if(keyPressed == KEY_UP)
            menu.selUp();
        else if(keyPressed == KEY_DOWN)
            menu.selDn();
        else if(keyPressed == KEY_LEFT)
            menu.selLeft();
        else if(keyPressed == KEY_RIGHT)
            menu.selRight();
        else if(keyPressed == KEY_RETURN)
            menu.select();
        else if(keyPressed == KEY_BACKSPACE || keyPressed == KEY_ESCAPE)
            menu.unselect();
        else if(keyPressed == '+')
            menu.incr();
        else if(keyPressed == '-')
            menu.decr();

        keyPressed = '\0';
    }
}







