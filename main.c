#include <stdio.h>
#include "modules/header.h"
#include <ncurses.h>

int main() {
    initscr();
    start_color();

    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);

    int program_mode = welcome_screen();

    clear();
    refresh();

    if (!program_mode){
        create();
    }
    else if (program_mode == 1) {
        edit(max_y, max_x);
    } else {
        printf("Manage Forms");
    }


    getchar();

    endwin();
    return 0;
}
