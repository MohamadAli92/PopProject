#include <stdio.h>
#include "modules/header.h"
#include <ncurses.h>

int main() {
    initscr();

    int program_mode = welcome_screen();

    clear();
    refresh();

    if (!program_mode){
        create();
    }
    else
        printf("Manage Forms");


    getchar();

    endwin();
    return 0;
}
