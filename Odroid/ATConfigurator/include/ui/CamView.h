//
// Created by jonas on 5/3/16.
//

#ifndef ATCONFIGURATOR_CAMVIEW_H
#define ATCONFIGURATOR_CAMVIEW_H

#include <ncurses.h>
#include "ui/ATCWindow.h"

namespace ui {
    class CamView : public ATCWindow {
    public:
        CamView(void);
        CamView(int, int);
        void refresh(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _camview;
    };
}

#endif //ATCONFIGURATOR_CAMVIEW_H
