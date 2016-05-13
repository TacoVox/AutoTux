/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#include "ui/State.h"
#include "od/ConferenceData.h"

ui::State::State(void) { State(80, 20); }

ui::State::State(int x, int y) : xsize(x - 2), ysize(y - 2),
                                 active(false), selectedItem(1),
                                 _state(newwin(ysize - 2, xsize - 16, 1, 16)) {
    mvwaddstr(_state, 1, 1, "State");
}

void ui::State::refresh(void) {
    //Clear the screen
    wmove(_state, 1, 1);
    wclrtobot(_state);

    printStates();
    wrefresh(_state);
}

void ui::State::select(void) {
    od::ConferenceData::STATE state =
            static_cast<od::ConferenceData::STATE>(selectedItem);
    od::ConferenceData::instance()->setState(state);

    active = true;
}

void ui::State::unselect(void) { active = false; }

void ui::State::selUp(void) {
    if(selectedItem == 0)
        selectedItem = 3;
    else
        selectedItem--;
}
void ui::State::selDn(void) {
    if(selectedItem == 3)
        selectedItem = 0;
    else
        selectedItem++;
}

void ui::State::selLeft(void) {}
void ui::State::selRight(void) {}
void ui::State::incr(void) {}
void ui::State::decr(void) {}

void ui::State::printStates(void) {
    if(selectedItem == 0 && active)
        wattron(_state, A_STANDOUT);
    mvwaddstr(_state, 1, 1, "Lane Following [ ]");
    wattroff(_state, A_STANDOUT);

    if(selectedItem == 1 && active)
        wattron(_state, A_STANDOUT);
    mvwaddstr(_state, 2, 1, "Driving        [ ]");
    wattroff(_state, A_STANDOUT);

    if(selectedItem == 2 && active)
        wattron(_state, A_STANDOUT);
    mvwaddstr(_state, 3, 1, "Parking        [ ]");
    wattroff(_state, A_STANDOUT);

    if(selectedItem == 3 && active)
        wattron(_state, A_STANDOUT);
    mvwaddstr(_state, 4, 1, "Resume         [ ]");
    wattroff(_state, A_STANDOUT);

    //Set the X
    int state = static_cast<int>(od::ConferenceData::instance()->getState());
    state++;
    mvwaddch(_state, state, 17, 'X');
}