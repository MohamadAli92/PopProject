
#include <stdio.h>
#include <ncurses.h>

int welcome_screen(){

    curs_set(0);

    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);

    move( 1, (max_x/2)-11 );

    attron(A_BOLD);

    printw("Welcome to form maker :)");

    attroff(A_BOLD);

    refresh();

    WINDOW * select_win = newwin(9, 30, (max_y/2)-4, (max_x/2)-14);

    box(select_win, '|', '-');
    init_pair(1,COLOR_CYAN, A_NORMAL);
    wbkgdset(select_win, COLOR_PAIR(1));

//    mvwprintw(select_win, 2, 7, "Create New Form");
//    mvwprintw(select_win, 4, 9, "Manage Forms");

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
    }

    keypad(select_win, false);
    return highlight;
//    wrefresh(select_win);
//    touchwin(select_win);



//    WINDOW * title_win = newwin(1, 24, max_y/4, max_x/4);
//
//    box(title_win, '-', '-');
//
//    wprintw(title_win, "Welcome to form maker :)");
//
//    refresh();

    //    char bo = '-';
//
//    wborder(title_win, bo, bo, bo, bo, bo, bo, bo, bo);
//
//    refresh();
//
//    wprintw(title_win, "Welcome to form maker :)");
//
//    wrefresh(title_win);
//
//    printf("Hi");


}