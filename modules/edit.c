#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>
#include <unistd.h>

void show_component_properties(int component_idx, WINDOW *properties, component *components){
    char type;
    component showing_component = components[component_idx];
        // Button Or Label Or TextBox
    if (showing_component.checked == -1){

        // Label
        if (showing_component.width == -1 || showing_component.height == -1)
            type = 'l';

        // TextBox
        else
            type = 't';

        // Button
    } else if (showing_component.checked == -5)
        type = 'b';

        // CheckBox
    else
        type = 'c';

    clear_window(properties, "");

    wmove(properties, 0, 0);

    wattron(properties, A_BOLD);
    wprintw(properties, "Type : ");
    wattroff(properties, A_BOLD);

    // Print types

    switch (type) {
        case 't':
            wprintw(properties, "TextBox\n");
            break;
        case 'b':
            wprintw(properties, "Button\n");
            break;
        case 'c':
            wprintw(properties, "CheckBox\n");
            break;
        case 'l':
            wprintw(properties, "Label\n");
            break;
        default:
            break;
    }

    wattron(properties, A_BOLD);
    wprintw(properties, "X : ");
    wattroff(properties, A_BOLD);

    wprintw(properties, "%d\n", showing_component.x_pos);

    wattron(properties, A_BOLD);
    wprintw(properties, "Y : ");
    wattroff(properties, A_BOLD);

    wprintw(properties, "%d\n", showing_component.y_pos);

    if (type == 't'){

        wattron(properties, A_BOLD);
        wprintw(properties, "Height : ");
        wattroff(properties, A_BOLD);

        wprintw(properties, "%d\n", showing_component.height);

        wattron(properties, A_BOLD);
        wprintw(properties, "Width : ");
        wattroff(properties, A_BOLD);

        wprintw(properties, "%d\n", showing_component.width);

    } else if (type == 'c'){

        wattron(properties, A_BOLD);
        wprintw(properties, "Checked by default : ");
        wattroff(properties, A_BOLD);

        if (showing_component.checked == 0)
            wprintw(properties, "YES\n");
        else
            wprintw(properties, "NO\n");

    }

    wrefresh(properties);
}


int select_from_components(int components_n, WINDOW *components_win, WINDOW *properties, component *components){
    int choice;
    int highlight = 0;
    keypad(components_win, true);

    int max_y_component, max_x_c;

    getmaxyx(components_win, max_y_component, max_x_c);

    int pages_number = components_n/max_y_component;
    if (!pages_number) pages_number = 1;
    int pages[pages_number][2];

    int counter = 0;
    for (int i = 0; i < components_n; ++i) {
        if (i % max_y_component == 0) {
            pages[counter][0] = i;
            if (i+max_y_component <= components_n)
                pages[counter][1] = i + max_y_component;

            else {
                pages[counter][1] = components_n;
                break;
            }
            counter++;

        }
    }

    int page_n = 0;
    while (1){
        wmove(components_win, 0, 0);
        for (int i = pages[page_n][0]; i < pages[page_n][1]; ++i) {
            if (i == highlight)
                wattron(components_win, A_REVERSE);
            wprintw(components_win, "%s\n", components[i].label_text);
            wattroff(components_win, A_REVERSE);
        }
        show_component_properties(highlight, properties, components);
        choice = wgetch(components_win);

        switch (choice) {
            case KEY_DOWN:
                highlight++;
                if (highlight == pages[page_n][1]){
                    if (highlight == components_n){
                        page_n = 0;
                        highlight = 0;
                    } else {
                        page_n++;
                    }
                    clear_window(components_win, "");
                }
                break;
            case KEY_UP:
                highlight--;
                if (highlight == pages[page_n][0]-1){
                    if (highlight == -1){
                        page_n = counter;
                        highlight = components_n-1;
                    } else{
                        page_n--;
                    }
                    clear_window(components_win, "");
                }
                break;
            default:
                break;
        }
        if (choice == '\n'){
            break;
        }
    }

    wrefresh(components_win);
    keypad(components_win, false);

    return highlight;
}

int delete_component(WINDOW *command, component *components, int component_idx, int components_n){
    for (int i = component_idx; i < components_n; ++i) {
        components[i] = components[i+1];
    }
    return components_n-1;
}

void edit(int max_y, int max_x){


    // initializing interface

    getmaxyx(stdscr, max_y, max_x);

    move(1, (max_x/2)-4);

    attron(A_BOLD);

    printw("Edit Form");
    attroff(A_BOLD);

    refresh();

    WINDOW *components_border = newwin(max_y-7, (max_x)/2, 3, 0);
    WINDOW *components_win = newwin(max_y-9, (max_x/2)-2, 4, 1);

    WINDOW *properties_border = newwin(max_y-7, max_x/2, 3, max_x/2);
    WINDOW *properties = newwin(max_y-9 , (max_x/2)-2, 4, (max_x/2)+1);

    WINDOW *command_border = newwin(4, max_x, max_y-4, 0);
    WINDOW *command = newwin(2, max_x-2, max_y-3, 1);


//    box(components_border, 0, 0);
    wborder(components_border, 0, '|', 0, '-', 0, 0, 0, 0);
    wborder(properties_border, '|', 0, 0, '-', 0, 0, 0, 0);
    wborder(command_border, 0, 0, '-', 0, 0, 0, 0, 0);

    wattron(components_border, A_BOLD | A_REVERSE);
    wattron(properties_border, A_BOLD | A_REVERSE);

    mvwprintw(components_border, 0, 1, "Components: ");
    mvwprintw(properties_border, 0, 1, "Properties: ");

    wattroff(properties_border, A_BOLD | A_REVERSE);
    wattroff(components_border, A_BOLD | A_REVERSE);

    wrefresh(components_border);
    wrefresh(components_win);
    wrefresh(properties_border);
    wrefresh(command_border);
    wrefresh(command);

    component *components = malloc(sizeof(component));
    int components_n;

    int file_not_found = 1;

    // open file

    while (file_not_found){

        // get file name from user

        curs_set(1);
        mvwprintw(command, 0, 0, "Please enter file name : ");
        char* file_name = malloc(116 * sizeof(char));
        wscanw(command, "%s", file_name);
        strcat(file_name, ".txt");


        // step 0 : getting components number

        int res;
        if ( (res = strcmp(file_name, ".txt") )){
            components_n = open_file(file_name, components, 0);

            components = realloc(components, components_n * sizeof(component) );

            // step 1 : getting components list

            open_file(file_name, components, 1);

        }

        if ( !res || (components->x_pos == -2 && components->y_pos == -1 && components_n == 1) ){
            curs_set(0);
            init_pair(1, COLOR_YELLOW, A_NORMAL);
            wattron(command, COLOR_PAIR(1));
            clear_window(command, "File not found;\nPlease try again.");
            wattroff(command, COLOR_PAIR(1));
            wrefresh(command);
            sleep(2);
            clear_window(command, "");
            wrefresh(command);
        }

        else {
            file_not_found = 0;
            break;
        }
    }

    curs_set(0);
    init_pair(1, COLOR_GREEN, A_NORMAL);
    wattron(command, COLOR_PAIR(1));
    clear_window(command, "File has been opened successfully.");
    wattroff(command, COLOR_PAIR(1));
    wrefresh(command);
    sleep(2);
    clear_window(command, "Please select one of the components to delete.");

    int component_idx = select_from_components(components_n, components_win, properties, components);

    components_n = delete_component(command, components, component_idx, components_n);


}
