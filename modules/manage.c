#include <stdio.h>
#include "header.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define ctrl(x)           ((x) & 0x1f)

typedef struct{
    int type;
    char data[1000];
} form_data;

void show_component_data(WINDOW *properties, form_data data){
    clear_window(properties, "");

    wmove(properties, 0, 0);

    wattron(properties, A_BOLD);
    wprintw(properties, "Type : ");
    wattroff(properties, A_BOLD);

    switch (data.type) {
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

    if (data.type == 't' || data.type == 'c'){
        wattron(properties, A_BOLD);
        wprintw(properties, "Data : ");
        wattroff(properties, A_BOLD);

        wprintw(properties, "%s\n", data.data);

    }


    wrefresh(properties);
}

int select_component(int components_n, WINDOW *components_win, WINDOW *properties, component *components, int *component_idx, form_data *all_data){
    int return_num;

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
            show_component_data(properties, all_data[highlight]);
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
            return_num = 0;
            break;
        } else if (choice == KEY_BACKSPACE){
            return_num = 1;
            break;
        } else if (choice == ctrl('s') || choice == ' '){
            return_num = -1;
            break;
        }
    }

    wrefresh(components_win);
    keypad(components_win, false);

    *component_idx = highlight;
    return return_num;

}

void get_add_data(WINDOW *command, form_data *data){
    curs_set(1);
    echo();
    keypad(command, TRUE);
    if (data->type == 't'){
        clear_window(command, "Please enter your data : ");
        wgetstr(command, data->data);
    } else if (data->type == 'c'){
        if (strcmp(data->data, "Checked") == 0){
            strcpy(data->data, "Unchecked");
        } else {
            strcpy(data->data, "Checked");
        }
    }
    clear_window(command, "");
    curs_set(0);
    keypad(command, FALSE);
    noecho();
}

void save_form_data(form_data *all_data, form_data **all_forms, int components_n, int forms_n){
    for (int i = 0; i < components_n; ++i) {
        all_forms[forms_n][i].type = all_data[i].type;
        strcpy(all_forms[forms_n][i].data, all_data[i].data);
    }
}

int add_new_form(int max_y, int max_x, component *components, int components_n, WINDOW *command, WINDOW* command_border, form_data *all_data, form_data **all_forms, int *forms_n, int f_type){
    int return_num;
    keypad(command, true);

    for (int i = 0; i < components_n; ++i) {
        char type;
        component showing_component = components[i];

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

        all_data[i].type = (unsigned char)type;
        if (type == 't')
            strcpy(all_data[i].data, "");
        else if (type == 'c'){
            if (showing_component.checked == 0){
                strcpy(all_data[i].data, "Checked");
            } else {
                strcpy(all_data[i].data, "Unchecked");
            }
        }
    }


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
    wrefresh(properties_border);

    while (1){
        if (f_type == 0){
            clear_window(command, "Press enter on a component to add data or press backspace to discard or Ctrl + s to save.");
        } else if (f_type == 1){
            clear_window(command, "Press enter on a component to add data or press backspace to discard or SPACE to search.");
        }
        int component_idx;
        int end;
        end = select_component(components_n, components_win, properties, components, &component_idx, all_data);
        clear_window(components_win, "");
        if (!end){
            get_add_data(command, (&all_data[component_idx]) );
        } else if (end == -1){
            if (f_type == 0){
                save_form_data(all_data, all_forms, components_n, *forms_n);
                (*forms_n)++;
            }
            return_num = -1;
            break;
        }
        else{
            return_num = 0;
            break;
        }
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

    return return_num;
}

void save_forms_to_file(form_data **all_forms, char* file_name, int forms_n, int components_n){

    char file_address[116];

    struct stat st = {0};

    if (stat("FormsData", &st) == -1) {
        mkdir("FormsData", 0700);
    }

    sprintf(file_address, "FormsData/%s", file_name);

    FILE *fp;
    fp = fopen(file_address, "wb+");

    form_data file_data_container;
    file_data_container.type = forms_n;

    fwrite(&file_data_container, sizeof(form_data), 1, fp);
    for (int i = 0; i < forms_n; ++i) {
        fwrite(all_forms[i], sizeof(form_data)*(components_n+1), 1, fp);
    }
    fclose(fp);

}

void show_form_data(WINDOW *properties, form_data *data, int components_n, component* components){
    clear_window(properties, "");

    wmove(properties, 0, 0);

    for (int i = 0; i < components_n; ++i) {
        wattron(properties, A_BOLD);
        wprintw(properties, "%s : ", components[i].label_text);
        wattroff(properties, A_BOLD);
        wprintw(properties, "%s\n", data[i].data);
    }

    wrefresh(properties);
}

int select_form_func(int forms_n, WINDOW *components_win, WINDOW *properties, form_data **all_forms, int *selected_form, int components_n, component *components, int form_indexes[], int deleting_forms[], int deleting_n){
    int return_num;

    int choice;
    int highlight = 0;
    keypad(components_win, true);

    int max_y_component, max_x_c;

    getmaxyx(components_win, max_y_component, max_x_c);

    int pages_number = forms_n/max_y_component;
    if (!pages_number) pages_number = 1;
    int pages[pages_number][2];

    int counter = 0;
    for (int i = 0; i < forms_n; ++i) {
        if (i % max_y_component == 0) {
            pages[counter][0] = i;
            if (i+max_y_component <= forms_n)
                pages[counter][1] = i + max_y_component;

            else {
                pages[counter][1] = forms_n;
                break;
            }
            counter++;

        }
    }

    int page_n = 0;
    while (1){
        wmove(components_win, 0, 0);
        for (int i = pages[page_n][0]; i < pages[page_n][1]; ++i) {
            int is_deleted = 0;
            for (int j = 0; j < deleting_n; ++j) {
                if (form_indexes[i] == deleting_forms[j])
                    is_deleted = 1;
            }
            if (i == highlight)
                wattron(components_win, A_REVERSE);
            if (is_deleted)
                wattron(components_win, A_BLINK);
            wprintw(components_win, "%d\n", form_indexes[i]+1);
            wattroff(components_win, A_REVERSE);
            wattroff(components_win, A_BLINK);
        }
        if (forms_n > 0)
            show_form_data(properties, all_forms[highlight], components_n, components);
        choice = wgetch(components_win);

        switch (choice) {
            case KEY_DOWN:
                highlight++;
                if (highlight == pages[page_n][1]){
                    if (highlight == forms_n){
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
                        highlight = forms_n-1;
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
            return_num = 0;
            break;
        } else if (choice == KEY_BACKSPACE){
            return_num = 1;
            break;
        } else if (choice == ' '){
            return_num = -1;
            break;
        } else if (choice == KEY_F(1)){
            return_num = -2;
            break;
        }
    }

    wrefresh(components_win);
    keypad(components_win, false);

    *selected_form = highlight;
    return return_num;



}

int search_for_forms(form_data *search_form, form_data **all_forms, int form_indexes[], int components_n, int forms_n){
    int idx = 0;
    int is_same = 1;
    for (int j = 0; j < forms_n; ++j) {
        is_same = 1;
        int res;
        for (int i = 0; i < components_n; ++i) {
            res = strcmp(search_form[i].data, "");
            if ( res != 0 ){
                res = strcmp(all_forms[j][i].data, search_form[i].data);
                if ( res != 0 ){
                    is_same = 0;
                    break;
                }
            }
        }
        if (is_same){
            form_indexes[idx] = j;
            idx++;
        }
    }
//    move(0,0);
//    for (int i = 0; i < idx; ++i) {
//        printw("%d", form_indexes[i]);
//    }
    return idx;
}

void delete_form(int form_idx, form_data **all_forms, int *forms_n){
    for (int i = form_idx; i < (*forms_n)+1; ++i) {
        all_forms[i] = all_forms[i+1];
    }
    if ((*forms_n) > 0)
        (*forms_n)--;
}

void edit_form(form_data *all_data, WINDOW *command, int max_y, int max_x, int components_n, component *components){
    keypad(command, true);

    WINDOW *components_border = newwin(max_y-7, (max_x)/2, 3, 0);
    WINDOW *components_win = newwin(max_y-9, (max_x/2)-2, 4, 1);

    WINDOW *properties_border = newwin(max_y-7, max_x/2, 3, max_x/2);
    WINDOW *properties = newwin(max_y-9 , (max_x/2)-2, 4, (max_x/2)+1);


    wborder(components_border, 0, '|', 0, '-', 0, 0, 0, 0);
    wborder(properties_border, '|', 0, 0, '-', 0, 0, 0, 0);

    wattron(components_border, A_BOLD | A_REVERSE);
    wattron(properties_border, A_BOLD | A_REVERSE);

    mvwprintw(components_border, 0, 1, "Components: ");
    mvwprintw(properties_border, 0, 1, "Properties: ");

    wattroff(properties_border, A_BOLD | A_REVERSE);
    wattroff(components_border, A_BOLD | A_REVERSE);

    wrefresh(components_border);
    wrefresh(properties_border);

    while (1){
        clear_window(command, "Press enter on a component to edit data or press BACKSPACE to go back.");
        int component_idx;
        int end;
        end = select_component(components_n, components_win, properties, components, &component_idx, all_data);
        clear_window(components_win, "");
        if (!end) {
            get_add_data(command, (&all_data[component_idx]));
        }
        else if (end == 1){
            break;
        }
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

}

void manage(int max_y, int max_x){

    echo();

    getmaxyx(stdscr, max_y, max_x);

    move(1, (max_x/2)-5);

    attron(A_BOLD);

    printw("Manage Forms");
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

    // Open file

    while (1){

        keypad(command, TRUE);

        // get file name from user

        curs_set(1);
        mvwprintw(command, 0, 0, "Please enter form's name : ");
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

    // Open data file

    struct stat st = {0};

    if (stat("FormsData", &st) == -1) {
        mkdir("FormsData", 0700);
    }

    char file_address[116];
    sprintf(file_address, "FormsData/%s", file_name);
    form_data file_data_container;
    form_data **all_forms = malloc(1000*sizeof(form_data*));
    int forms_n;

    FILE *data_file;
    data_file = fopen(file_address, "rb+");

    for (int i = 0; i < 1000; ++i) {
        all_forms[i] = malloc(sizeof(form_data)*(components_n+1));
    }

    if (!data_file){
        data_file = fopen(file_address, "wb+");
        forms_n = 0;
    } else {
        fread(&file_data_container, sizeof(form_data), 1, data_file);
        forms_n = file_data_container.type;
        for (int i = 0; i < forms_n; ++i) {
            fread(all_forms[i], sizeof(form_data)*(components_n+1), 1, data_file);
            for (int j = 0; j < components_n; ++j) {
                form_data checking_data = all_forms[i][j];
                if (checking_data.type != 't' && checking_data.type != 'c' && checking_data.type != 'b' && checking_data.type != 'l'){
                    if (components[j].width == -1 && components[j].height == -1){
                        if (components[j].checked == -5){
                            // button
                            all_forms[i][j].type = 'b';
                        } else if (components[j].checked == -1) {
                            // Label
                            all_forms[i][j].type = 'l';
                        } else {
                            // CheckBox
                            all_forms[i][j].type = 'c';
                        }
                    } else if (components[j].checked == -1){
                        // TextBox
                        all_forms[i][j].type = 't';
                    }
                }
            }
        }
    }

    int choice = 0;

    while (choice != 2) {


        clear_window(command, "Select one of these commands.");
        char choices[3][7] = {"|Add|", "|Find|", "|Exit|"};

        choice = get_choice(max_x, 7, choices, command, 3, 24, 1, 0);

        if (choice == 0){
            form_data *all_data = malloc((components_n+1)*sizeof(form_data));

            clear_window(command, "Press enter on a component to add data or press backspace to discard or CTRL+S to save form.");
            cbreak();
            add_new_form(max_y, max_x, components, components_n, command, command_border, all_data, all_forms, &forms_n, 0);
        } else if (choice == 2){
            clear_window(command, "Do you want to save changes?");
            char choices_exit[2][5] = {"YES", "NO"};
            int save = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 1, 0);
            fclose(data_file);
            if (!save){
                save_forms_to_file(all_forms, file_name, forms_n, components_n);
            }
        } else if (choice == 1){
            if (forms_n == 0){
                init_pair(1, COLOR_YELLOW, A_NORMAL);
                wattron(command, COLOR_PAIR(1));
                clear_window(command, "There isn't any form to search for;\nPlease first add forms.");
                wattroff(command, COLOR_PAIR(1));
                wrefresh(command);
                sleep(2);
                clear_window(command, "");
                wrefresh(command);
            } else {
                form_data *search_form = malloc((components_n)*sizeof(form_data));
                clear_window(command, "Please press enter on any component to fill it with desired data and"
                                      " then press SPACE to search for forms.");
                int search_cond = add_new_form(max_y, max_x, components, components_n, command, command_border, search_form, all_forms, &forms_n, 1);
                // should search
                if (search_cond == -1){

                    int form_indexes[1000];


                    int searched_forms_n = search_for_forms(search_form, all_forms, form_indexes, components_n, forms_n);


                    form_data **all_searched_forms = malloc(searched_forms_n*sizeof(form_data*));
                    for (int i = 0; i < searched_forms_n; ++i) {
                        all_searched_forms[i] = malloc((components_n+1)*sizeof(form_data));
                        all_searched_forms[i] = all_forms[form_indexes[i]];
                    }

                    WINDOW *components_border = newwin(max_y-7, (max_x)/2, 3, 0);
                    WINDOW *components_win = newwin(max_y-9, (max_x/2)-2, 4, 1);

                    WINDOW *properties_border = newwin(max_y-7, max_x/2, 3, max_x/2);
                    WINDOW *properties = newwin(max_y-9 , (max_x/2)-2, 4, (max_x/2)+1);


                    int deleting_forms[1000];
                    int deleting_n = 0;

                    while (1){
                        clear_window(command, "Press Enter on forms to delete or F1 to edit or BACKSPACE to discard and space to save changes.");
                        wborder(components_border, 0, '|', 0, '-', 0, 0, 0, 0);
                        wborder(properties_border, '|', 0, 0, '-', 0, 0, 0, 0);
                        wborder(command_border, 0, 0, '-', 0, 0, 0, 0, 0);

                        wattron(components_border, A_BOLD | A_REVERSE);
                        wattron(properties_border, A_BOLD | A_REVERSE);

                        mvwprintw(components_border, 0, 1, "Forms: ");
                        mvwprintw(properties_border, 0, 1, "Components: ");

                        wattroff(properties_border, A_BOLD | A_REVERSE);
                        wattroff(components_border, A_BOLD | A_REVERSE);

                        wrefresh(components_border);
                        wrefresh(properties_border);

                        int end;
                        int selected_form;

                        end = select_form_func(searched_forms_n, components_win, properties, all_searched_forms, &selected_form, components_n, components, form_indexes, deleting_forms, deleting_n);

                        if (end == 0){
                            int check_if_exist = 0;
                            for (int i = 0; i < deleting_n; ++i) {
                                if (selected_form == deleting_forms[i]){
                                    check_if_exist = 1;
                                    break;
                                }
                            }
                            if (check_if_exist == 0){
                                deleting_forms[deleting_n] = form_indexes[selected_form];
                                deleting_n++;
                            }
                        } else if (end == -1){
                            for (int i = deleting_n-1; i >= 0; --i) {
                                delete_form(deleting_forms[i], all_forms, &forms_n);
                            }
                            break;
                        }
                        else if (end == -2){
                            edit_form(all_forms[form_indexes[selected_form]], command, max_y, max_x, components_n, components);
                        } else {
                            break;
                        }
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

                }

            }

        }
    }
}