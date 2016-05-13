/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Main UI class which spawns all the other sub windows. Uses ncurses.
 */

#include "ui/MainUI.h"
#include "ui/ValMonitor.h"
#include "ui/State.h"
#include "ui/CamSettings.h"
#include "ui/CamView.h"
#include <thread>

/**
 * Default constructor. Inits the default window, sets the sizes, interrupted to
 * false as well as setting to key pressed to \0.
 */
ui::MainUI::MainUI(void) : _window(initscr()),
                           xsize(getmaxx(_window)), ysize(getmaxy(_window)),
                           interrupted(false), keyPressed((int)'\0') {
    //Set the cursors position to 0
    curs_set(0);
    //Enable coloring
    start_color();
    //Enable keypad usage
    keypad(_window, true);
    //Use nodelay mode to read in keys without blocking
    nodelay(_window, true);
    //Refresh the window
    refresh();

    //Init sub windows
    menu = Menu(xsize, ysize);
    header = Header(xsize, ysize);
    footer = Footer(xsize, ysize);
    windows.push_back((std::unique_ptr<ATCWindow>)new ValMonitor(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new State(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamSettings(xsize, ysize));
    windows.push_back((std::unique_ptr<ATCWindow>)new CamView(xsize, ysize));
}

/**
 * Main loop which runs in a seperate thread.
 */
void ui::MainUI::mainLoop(void) {
    while(!interrupted) {
        //Read in the char pressed
        keyPressed = getch();

        //Refresh all the sub windows
        header.refresh();
        menu.refresh();
        if(menu.getCurrent() != 4)
            windows.at(menu.getCurrent())->refresh();
        else
            windows.at(3)->refresh();
        footer.refresh();

        //Handle the keys pressed
        handleKeys();

        //Sleep for a while to NOT drive the CPU nuts
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

/**
 * Quit method to shut down the ncurses part of the application correctly-
 */
void ui::MainUI::quit(void) {
    interrupted = true;
    endwin();
}

/**
 * Method to handle key input and do the correct actions at the correct time.
 */
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
        } else if(keyPressed == KEY_RETURN && menu.getCurrent() == 4) {
            quit();
        } else if(keyPressed == KEY_RETURN) {
            menu.setActive(false);
            windows.at(menu.getCurrent())->select();
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







