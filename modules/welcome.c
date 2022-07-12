
#include <stdio.h>
#include <ncurses.h>

int welcome_screen(){

    noecho();

    curs_set(0);

    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);

    move( 1, (max_x/2)-4 );

    attron(A_BOLD);

    printw("Form Maker");

    attroff(A_BOLD);

    init_pair(5,COLOR_YELLOW, A_NORMAL);

    attron(COLOR_PAIR(5));
    mvprintw(max_y-2, 0, "F1 = Help");
    mvprintw(max_y-1, 0, "F2 = Adjust form size");
    attroff(COLOR_PAIR(5));


    refresh();

    WINDOW * select_win = newwin(9, 30, (max_y/2)-4, (max_x/2)-14);

    box(select_win, '|', '-');
    init_pair(1,COLOR_CYAN, A_NORMAL);
    wbkgdset(select_win, COLOR_PAIR(1));


    int choice;
    int highlight = 0;
    keypad(select_win, true);

    while (1) {
        if (!highlight){
            wattron(select_win, A_REVERSE);
            mvwprintw(select_win, 1, 1, "Create New Form");
            wattroff(select_win, A_REVERSE);
            mvwprintw(select_win, 3, 1, "Manage Forms");
            mvwprintw(select_win, 5, 1, "Edit Form");
            mvwprintw(select_win, 7, 1, "Quit");
        } else if (highlight == 1){
            mvwprintw(select_win, 1, 1, "Create New Form");
            wattron(select_win, A_REVERSE);
            mvwprintw(select_win, 3, 1, "Manage Forms");
            wattroff(select_win, A_REVERSE);
            mvwprintw(select_win, 5, 1, "Edit Form");
            mvwprintw(select_win, 7, 1, "Quit");
        } else if (highlight == 2){
            mvwprintw(select_win, 1, 1, "Create New Form");
            mvwprintw(select_win, 3, 1, "Manage Forms");
            wattron(select_win, A_REVERSE);
            mvwprintw(select_win, 5, 1, "Edit Form");
            wattroff(select_win, A_REVERSE);
            mvwprintw(select_win, 7, 1, "Quit");
        } else if (highlight == 3){
            mvwprintw(select_win, 1, 1, "Create New Form");
            mvwprintw(select_win, 3, 1, "Manage Forms");
            mvwprintw(select_win, 5, 1, "Edit Form");
            wattron(select_win, A_REVERSE);
            mvwprintw(select_win, 7, 1, "Quit");
            wattroff(select_win, A_REVERSE);
        }

        wrefresh(select_win);

        choice = wgetch(select_win);

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1) highlight = 3;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight == 4) highlight = 0;
                break;
            default:
                break;
        }
        if (choice == '\n') break;
        if (choice == KEY_F(2)) {
            keypad(select_win, false);
            return -2;
        }
    }

    keypad(select_win, false);
    return highlight;


}