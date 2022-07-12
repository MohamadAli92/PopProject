#include <stdio.h>
#include "modules/header.h"
#include <ncurses.h>

int main() {
    initscr();
    start_color();

    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);

    int program_mode = 0;

    while (program_mode != 3){
        clear();
        refresh();
        program_mode = welcome_screen();
        clear();
        refresh();

        if (program_mode == 0){
            create();
        }
        else if (program_mode == 1) {
            manage(max_y,max_x);
        } else if (program_mode == 2){
            edit(max_y, max_x);
        } else if (program_mode == -2){
            adjust();
        }

    }




    endwin();
    return 0;
}
