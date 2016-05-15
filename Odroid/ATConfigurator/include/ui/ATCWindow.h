/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Pure virtual class for all the AutoTuxConfigurator windows.
 */

#ifndef ATCONFIGURATOR_ATCWINDOW_H
#define ATCONFIGURATOR_ATCWINDOW_H

namespace ui {
    class ATCWindow {
    public:
        // Method for refreshing the window
        virtual void refresh(void) = 0;
        // Method for selecting an element in the window
        virtual void select(void) = 0;
        // Method for unselecting the window
        virtual void unselect(void) = 0;
        // Method for going up one element in the window
        virtual void selUp(void) = 0;
        // Method for going down one element in the window
        virtual void selDn(void) = 0;
        // Method for going left one element in the window
        virtual void selLeft(void) = 0;
        // Method for going right one element in the window
        virtual void selRight(void) = 0;
        // Method for increasing a value in the window
        virtual void incr(void) = 0;
        // Method for decreasing a value in the window
        virtual void decr(void) = 0;
    };
}

#endif //ATCONFIGURATOR_ATCWINDOW_H
