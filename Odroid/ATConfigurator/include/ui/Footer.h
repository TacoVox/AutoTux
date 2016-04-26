//
// Created by jonas on 4/26/16.
//

#ifndef ATCONFIGURATOR_FOOTER_H
#define ATCONFIGURATOR_FOOTER_H

#include <ncurses.h>
#include <memory>

namespace ui {
    class Footer {
    public:
        Footer(int, int);
        WINDOW* getFooter(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _footer;
        void dateLoop(void);
    };
}

#endif //ATCONFIGURATOR_FOOTER_H
