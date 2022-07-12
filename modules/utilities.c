#include <ncurses.h>

#include <string.h>

#include "header.h"

void clear_window(WINDOW * win, char text[]) {
    wclear(win);
    wattron(win, A_BOLD);
    mvwprintw(win, 0, 0, "%s", text);
    wattroff(win, A_BOLD);
    wrefresh(win);
}

int get_choice(int max_x, size_t str_max_len, char choices[][str_max_len], WINDOW * win, int choices_n, int all_lens, int start_y, int start_x) {

    curs_set(0);
    noecho();

    int lens[choices_n];
    int temp = 0;
    for (int i = 0; i < choices_n; ++i) {
        lens[i] = temp + (max_x - all_lens) / (choices_n - 1) + (int) strlen(choices[i]);
        temp = lens[i];
    }
    int choice;
    int highlight = 0;
    keypad(win, true);

    while (1) {
        wmove(win, start_y, start_x);
        for (int i = 0; i < choices_n; ++i) {
            if (i == highlight)
                wattron(win, A_REVERSE);
            wprintw(win, "%s", choices[i]);
            wmove(win, start_y, lens[i]);
            wattroff(win, A_REVERSE);
        }
        choice = wgetch(win);

        switch (choice) {
            case KEY_LEFT:
                highlight--;
                if (highlight == -1)
                    highlight = choices_n - 1;
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

void save_file(component * components, int * components_n, FILE * fp) {
    component file_data;
    file_data.x_pos = -1;
    file_data.y_pos = -1;
    file_data.width = ( * components_n);
    for (int i = ( * components_n); i > 0; --i) {
        components[i] = components[i - 1];
    }
    components[0] = file_data;
    fwrite(components, sizeof(component), ( * components_n) + 1, fp);
    fclose(fp);
}

int open_file(char * file_name, component * components, int step) {
    FILE * fp;
    char file_address[116] = "FormsTemplates/";
    strcat(file_address, file_name);
    fp = fopen(file_address, "rb+");
    int components_n;
    if (!fp) {
        components -> y_pos = -1;
        components -> x_pos = -2;
        return 1;
    } else {
        component file_data;

        fread( & file_data, sizeof(component), 1, fp);

        components_n = file_data.width;

        if (step == 0)
            return components_n;

        component my_comp;

        for (int i = 0; i < components_n; ++i) {
            fread( & my_comp, sizeof(component), 1, fp);
            components[i] = my_comp;
        }
        fclose(fp);
        return 0;
    }
}