/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#ifndef ATCONFIGURATOR_MAINUI_H
#define ATCONFIGURATOR_MAINUI_H

#define KEY_RETURN 10 //Override non working ncurses value
#define KEY_ESCAPE 27

#include <string>
#include <ncurses.h>
#include <memory>
#include "ui/Header.h"
#include "ui/Footer.h"
#include "ui/Menu.h"

namespace ui {
    class MainUI {
    public:
        MainUI(void);
        void mainLoop(void);
        void quit(void);
    private:
        WINDOW* _window;
        int xsize;
        int ysize;
        bool interrupted;
        int keyPressed;
        Header header;
        Footer footer;
        Menu menu;
        std::vector<std::unique_ptr<ui::ATCWindow>> windows;
        void handleKeys(void);
    };
}

#endif //ATCONFIGURATOR_MAINUI_H
