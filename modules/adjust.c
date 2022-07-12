#include <stdio.h>
#include "ncurses.h"
#include "header.h"
void adjust(){
    int y, x;
    int k = 'k';
    while (1) {
        endwin();
        initscr();
        getmaxyx(stdscr, y, x);
        clear();
        mvprintw(0, 0, "Your form size is :\nY : %d\nX : %d\nPress SPACE to confirm.", y-9, x);
        refresh();
        if (k == ' ') {
            break;
        } else {
            clear();
            WINDOW *view_border = newwin(y - 9, x, 4, 0);
            box(view_border, '|', '-');

            wrefresh(view_border);

            wclear(view_border);
            delwin(view_border);
        }
        k = getchar();
        refresh();
    }
}
