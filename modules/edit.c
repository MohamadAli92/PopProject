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

void delete_component(component *components, int component_idx, int components_n){
    for (int i = component_idx; i < components_n; ++i) {
        components[i] = components[i+1];
    }
}


int add_component_func(int option, WINDOW *command, WINDOW *view, char choices[][13], int points[][2], int max_x,
                   int* n_p_point, component *components, int *components_p_n, int view_max_x, int view_max_y) {

    int confirmed_component = 0;
    component created_component;


    int total_height;
    int total_width;

    while (!confirmed_component && option != 4){

        keypad(command, TRUE);

        clear_window(command, choices[option]);

        echo();

        curs_set(1);

        do {
            clear_window(command, choices[option]);
            mvwprintw(command, 1, 0, "|Label text (maximum 20 characters) : ");
            wgetstr(command, created_component.label_text);
        } while (strlen(created_component.label_text) > 20);

        do {
            clear_window(command, choices[option]);
            mvwprintw(command, 1, 0, "|X Position (only number and maximum %d) : ", view_max_x);
            wscanw(command, "%d", &created_component.x_pos);
        } while (created_component.x_pos >= view_max_x);

        do {
            clear_window(command, choices[option]);
            mvwprintw(command, 1, 0, "|Y Position (only number and maximum %d) : ", view_max_y);
            wscanw(command, "%d", &created_component.y_pos);
        } while (created_component.y_pos >= view_max_y);

        clear_window(command, choices[option]);

        if (option == 0){

            // TextBox

            created_component.checked = -1;

            do {
                clear_window(command, choices[option]);
                mvwprintw(command, 1, 0, "|Width & Height of textbox (only numbers and maximums are %d & %d) : ", view_max_x-created_component.x_pos-2, view_max_y-created_component.y_pos-2);
                wscanw(command, "%d %d", &created_component.width, &created_component.height);
            } while (created_component.width > view_max_x-created_component.x_pos-2 || created_component.height > view_max_y-created_component.y_pos-2);

            clear_window(command, choices[option]);

            mvwprintw(command, 0, 0, "label : %s | x_pos : %d | y_pos : %d | Width : %d | Height : %d", created_component.label_text, created_component.x_pos, created_component.y_pos, created_component.width, created_component.height);

            total_height = created_component.height+2;
            total_width = created_component.width+2;

        }
        else if (option == 2){

            // CheckBox

            keypad(command, FALSE);

            created_component.width = -1;
            created_component.height = -1;

            curs_set(0);
            noecho();
            mvwprintw(command, 1, 0, "Is checkbox ,checked by default?\n ");
            char choices_exit[2][5] = {"YES", "NO"};
            created_component.checked = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 2, 1);
            clear_window(command, choices[option]);
            mvwprintw(command, 0, 0, "label : %s | x_pos : %d | y_pos : %d | Checked by default : %s", created_component.label_text, created_component.x_pos, created_component.y_pos, !created_component.checked ? "YES" : "NO");

            total_height = 3;
            total_width = (int) strlen(created_component.label_text)+5;

        } else {

            // Button, Label

            created_component.checked = -1;
            created_component.height = -1;
            created_component.width = -1;
            if (option == 1)
                // Button
                created_component.checked = -5;
                // Label
            else created_component.checked = -1;

            mvwprintw(command, 0, 0, "label : %s | x_pos : %d | y_pos : %d", created_component.label_text, created_component.x_pos, created_component.y_pos);

            if (option == 1){
                total_height = 3;
                total_width = (int)strlen(created_component.label_text)+2;
            } else {
                total_height = 1;
                total_width = (int)strlen(created_component.label_text);
            }
        }


        int is_in_lines = 0;
        int is_in_cols = 0;

        int c_p_n = 0;

        int component_points[total_height*total_width][2];

        for (int j = 0; j < total_height; ++j) {
            for (int i = 0; i < total_width; ++i) {
                component_points[c_p_n][0] = created_component.y_pos+j;
                component_points[c_p_n][1] = created_component.x_pos+i;
                c_p_n++;
            }
        }


        for (int i = 0; i < (*n_p_point); ++i) {
            for (int j = 0; j < c_p_n; ++j) {
                is_in_lines = 0;
                is_in_cols = 0;
                if (component_points[j][0] == points[i][0])
                    is_in_lines = 1;
                if (component_points[j][1] == points[i][1])
                    is_in_cols = 1;
                if (is_in_cols && is_in_lines)
                    break;
            }
            if (is_in_lines && is_in_cols)
                break;
        }



        int overlapped;
        if (is_in_cols && is_in_lines) {
            curs_set(0);
            clear_window(command, "This component will overlap with another one;\nPlease try again.");
            wrefresh(command);
            overlapped = 1;
            sleep(3);
            clear_window(command, "");
        } else overlapped = 0;

        if (!overlapped){
            keypad(command, FALSE);
            clear_window(command, "Do you want to create this component?\n ");
            char choices_exit[2][5] = {"YES", "NO"};
            confirmed_component = !( get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 1, 1) );
            if (confirmed_component){
                for (int i = 0; i < c_p_n; ++i) {
                    points[(*n_p_point)][0] = component_points[i][0];
                    points[(*n_p_point)][1] = component_points[i][1];
                    (*n_p_point)++;
                }
            }
        }

    }

    // Print component on view window
    put_component_on_view(option, created_component, view);

    // Add component to the array
    if (option != 4){
        components[(*components_p_n)] = created_component;
        (*components_p_n)++;
    }


    if (option == 4) {
        return 1;
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

        keypad(command, TRUE);
        curs_set(1);
        mvwprintw(command, 0, 0, "Please enter file name : ");
        wgetstr(command, file_name);
        strcat(file_name, ".txt");


        // step 0 : getting components number

        int res;
        if ( (res = strcmp(file_name, ".txt") )){
            components_n = open_file(file_name, components, 0);

            if (components_n != 0){
                components = realloc(components, components_n * sizeof(component) );

                // step 1 : getting components list

                open_file(file_name, components, 1);
            } else {
                res = 0;
            }


        }

        if ( !res || (components->x_pos == -2 && components->y_pos == -1 && components_n == 1) ){
            curs_set(0);
            init_pair(1, COLOR_YELLOW, A_NORMAL);
            wattron(command, COLOR_PAIR(1));
            clear_window(command, "File not found or it's an empty form;\nPlease try again.");
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
    keypad(command, FALSE);
    char save_file_name[116];
    strcpy(save_file_name, file_name);

    curs_set(0);
    init_pair(1, COLOR_GREEN, A_NORMAL);
    wattron(command, COLOR_PAIR(1));
    clear_window(command, "File has been opened successfully.");
    wattroff(command, COLOR_PAIR(1));
    wrefresh(command);
    sleep(2);

    noecho();

    attron(A_BOLD);
    attron(A_ITALIC);
    attron(A_REVERSE);
    mvprintw(0,0, "%s", file_name);
    refresh();
    attroff(A_REVERSE);
    attroff(A_ITALIC);
    attroff(A_BOLD);

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
                    delete_component(components, component_idx, components_n);
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
                strcat(file_address, save_file_name);
                fpo = fopen(file_address, "wb+");
                save_file(components, &components_n, fpo);
            }

        } else if (choice == 0) {

            WINDOW *view = newwin(max_y-11, max_x-2, 4, 1);
            WINDOW *view_border = newwin(max_y-9, max_x, 3, 0);

            box(view_border, '|', '-');

            wrefresh(view_border);
            wrefresh(view);

            int view_max_y, view_max_x;
            getmaxyx(view, view_max_y, view_max_x);
            int points[view_max_x*view_max_y][2];

            int end = 0;
            char create_choices[5][13] = {"|TextBox|", "|Button|", "|Check Box|", "|Label|", "|Exit|"};

            int n_point = 0;
            for (int i = 0; i < components_n; ++i) {

                int total_height;
                int total_width;

                int type;

                component checking_comp = components[i];
                if (checking_comp.width == -1 && checking_comp.height == -1){
                    if (checking_comp.checked == -5){
                        // button
                        total_height = 3;
                        total_width = (int)strlen(checking_comp.label_text)+2;
                        type = 1;
                    } else if (checking_comp.checked == -1) {
                        // Label
                        total_height = 1;
                        total_width = (int)strlen(checking_comp.label_text);
                        type = 3;
                    } else {
                        // CheckBox
                        total_height = 3;
                        total_width = (int) strlen(checking_comp.label_text)+5;
                        type = 2;
                    }
                } else if (checking_comp.checked == -1){
                    // TextBox
                    total_height = checking_comp.height+2;
                    total_width = checking_comp.width+2;
                    type = 0;
                }

                put_component_on_view(type, checking_comp, view);

                int c_p_n = 0;

                int component_points[total_height*total_width][2];

                for (int j = 0; j < total_height; ++j) {
                    for (int p = 0; p < total_width; ++p) {
                        component_points[c_p_n][0] = checking_comp.y_pos+j;
                        component_points[c_p_n][1] = checking_comp.x_pos+p;
                        c_p_n++;
                    }
                }

                for (int p = 0; p < c_p_n; ++p) {
                    points[n_point][0] = component_points[p][0];
                    points[n_point][1] = component_points[p][1];
                    n_point++;
                }

            }

            while (!end){
                clear_window(command, "Please choose one to add on form.");
                
                
                int option = get_choice(max_x, 13, create_choices, command, 5, 48, 1, 0);

                end = add_component_func(option, command, view, create_choices, points, max_x, &n_point, components, &components_n, view_max_x, view_max_y);

            }

        }

    }

}
