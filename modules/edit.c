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


int select_from_components(int components_n, WINDOW *components_win, WINDOW *properties, component *components, int *component_idx){
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
        if (components_n > 0)
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
        } else if (choice == KEY_BACKSPACE){
            return 1;
        }
    }

    wrefresh(components_win);
    keypad(components_win, false);

    *component_idx = highlight;
    return 0;
}

void delete_component(WINDOW *command, component *components, int component_idx, int components_n){
    for (int i = component_idx; i < components_n; ++i) {
        components[i] = components[i+1];
    }
}

void edit(int max_y, int max_x){

    echo();

    getmaxyx(stdscr, max_y, max_x);

    move(1, (max_x/2)-4);

    attron(A_BOLD);

    printw("Edit Form");
    attroff(A_BOLD);

    refresh();


    WINDOW *command_border = newwin(4, max_x, max_y-4, 0);
    WINDOW *command = newwin(2, max_x-2, max_y-3, 1);
    box(command_border, '|', '-');

    wrefresh(command_border);
    wrefresh(command);

    // Load File

    component *components = malloc(sizeof(component));
    int components_n;
    char* file_name = malloc(116 * sizeof(char));

    // open file

    while (1){

        // get file name from user

        curs_set(1);
        mvwprintw(command, 0, 0, "Please enter file name : ");
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

    noecho();

    int choice = 0;

    while (choice != 3){

        clear_window(command, "Select one of these commands.");
        char choices[4][19] = {"|Add Component|", "|Delete Component|", "|View Form|", "|Exit|"};

        choice = get_choice(max_x, 19, choices, command, 4, 57, 1, 0);

        if (choice == 2){

            keypad(command, true);
            clear_window(command, "Press Key Down to exit.");

            WINDOW *view = newwin(max_y-11, max_x-2, 4, 1);
            WINDOW *view_border = newwin(max_y-9, max_x, 3, 0);

            box(view_border, '|', '-');

            wrefresh(view_border);
            wrefresh(view);

            for (int i = 0; i < components_n; ++i) {
                int type;
                component showing_component = components[i];
                // Button Or Label Or TextBox
                if (showing_component.checked == -1){

                    // Label
                    if (showing_component.width == -1 || showing_component.height == -1)
                        type = 3;

                        // TextBox
                    else
                        type = 0;

                    // Button
                } else if (showing_component.checked == -5)
                    type = 1;

                    // CheckBox
                else
                    type = 2;

                put_component_on_view(type, showing_component, view);

            }


            int exit = KEY_UP;
            while (exit != KEY_DOWN){
                exit = wgetch(command);
            }


            keypad(command, false);

            clear_window(view_border, "");
            clear_window(view, "");


        } else if (choice == 1){
            // Delete Component

            clear_window(command, "|Delete|");

            // initializing interface


            WINDOW *components_border = newwin(max_y-7, (max_x)/2, 3, 0);
            WINDOW *components_win = newwin(max_y-9, (max_x/2)-2, 4, 1);

            WINDOW *properties_border = newwin(max_y-7, max_x/2, 3, max_x/2);
            WINDOW *properties = newwin(max_y-9 , (max_x/2)-2, 4, (max_x/2)+1);


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

            clear_window(command, "Please select one of the components to delete or press BackSpace to exit.");

            while (components_n > 0){
                int component_idx;
                int end;
                end = select_from_components(components_n, components_win, properties, components, &component_idx);
                clear_window(components_win, "");
                if (!end){
                    delete_component(command, components, component_idx, components_n);
                    components_n--;
                }
                else
                    break;
            }

            if (!components_n){
                clear_window(command, "There is no component to delete.");
                sleep(2);
            }


            clear_window(components_win, "");
            clear_window(components_border, "");
            clear_window(properties, "");
            clear_window(properties_border, "");

            delwin(components_border);
            delwin(components_win);
            delwin(properties_border);
            delwin(properties);
            refresh();

        } else if (choice == 3){
            clear_window(command, "|Do you want to save created form?");
            char choices_exit[2][5] = {"YES", "NO"};
            int save;
            save = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 1, 1);
            if (!save){
                FILE *fpo;
                char file_address[116] = "FormsTemplates/";
                strcat(file_address, file_name);
                fpo = fopen(file_address, "wb+");
//                mvprintw(0,0,"%s", file_address);
//                refresh();
                save_file(components, components_n, fpo);
            }

        } else if (choice == 0) {
            FILE *fpo;

            char file_address[116] = "FormsTemplates";
            strcat(file_address, file_name);
            fpo = fopen(file_address, "wb+");

            WINDOW *view = newwin(max_y-11, max_x-2, 4, 1);
            WINDOW *view_border = newwin(max_y-9, max_x, 3, 0);

            box(view_border, '|', '-');

            wrefresh(view_border);
            wrefresh(view);

            int view_max_y, view_max_x;
            getmaxyx(view, view_max_y, view_max_x);
            int points[view_max_x*view_max_y][2];

            clear_window(command, "");
            wrefresh(command);

            int end = 0;
            char create_choices[5][13] = {"|TextBox|", "|Button|", "|Check Box|", "|Label|", "|Exit|"};

            while (!end){
                clear_window(command, "\0");

                mvwprintw(command, 0, 0, "Please choose one to add on form.");

                int option;
                option = get_choice(max_x, 13, choices, command, 5, 48, 1, 0);

                wclear(command);
                end = process_option(option, command, view, create_choices, points);

            }

            save_file(components, components_n, fpo);

        }

    }

}
