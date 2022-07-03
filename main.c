#include <stdio.h>
#include "modules/header.h"
#include <ncurses.h>

int main() {
    initscr();

    int program_mode = welcome_screen();

    clear();
    refresh();
    curs_set(1);

    if (!program_mode)
        printf("Create New Form");
    else
        printf("Manage Forms");


    getchar();

    endwin();
    return 0;
}
