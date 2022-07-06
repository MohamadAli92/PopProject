
#include <ncurses.h>
#include <string.h>


int get_choice(int max_x, size_t str_max_len, char choices[][str_max_len], WINDOW* win, int choices_n, int all_lens, int start_y, int start_x){

    curs_set(0);
    noecho();

    int lens[choices_n];
    int temp = 0;
    for (int i = 0; i < choices_n; ++i) {
        lens[i] = temp + (max_x-all_lens)/(choices_n-1) + (int)strlen(choices[i]);
        temp = lens[i];
    }
    int choice;
    int highlight = 0;
    keypad(win, true);

    while (1){
        wmove(win, start_y, start_x);
        for (int i = 0; i < choices_n; ++i) {
            if (i == highlight)
                wattron(win, A_REVERSE);
            wprintw(win, "%s", choices[i]);
            wmove(win ,start_y, lens[i]);
            wattroff(win, A_REVERSE);
        }
        choice = wgetch(win);

        switch (choice) {
            case KEY_LEFT:
                highlight--;
                if (highlight == -1)
                    highlight = choices_n-1;
                break;
            case KEY_RIGHT:
                highlight++;
                if (highlight == choices_n)
                    highlight = 0;
                break;
            default:
                break;
        }
        if (choice == '\n')
            break;
    }

    wrefresh(win);
    keypad(win, false);

    return highlight;
}