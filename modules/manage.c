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
            break;
        } else if (choice == KEY_BACKSPACE){
            return 1;
        } else if (choice == ctrl('s')){
            return -1;
        }
    }

    wrefresh(components_win);
    keypad(components_win, false);

    *component_idx = highlight;
    return 0;

}

void get_add_data(WINDOW *command, form_data *data){
    curs_set(1);
    echo();
    clear_window(command, "Please enter your data : ");
    wscanw(command, "%s", data->data);
    clear_window(command, "Press enter on a component to add data or press backspace to discard or Ctrl + s to save.");
    curs_set(0);
    noecho();
}

void save_form_data(form_data *all_data, form_data **all_forms, int components_n, int forms_n){
    for (int i = components_n; i > 0; --i) {
        all_data[i] = all_data[i-1];
    }
    char num[10];
    sprintf(num, "%d", forms_n);
    strcpy(all_data[0].data, num);
    for (int i = 1; i < components_n+1; ++i) {
        all_forms[forms_n][i].type = all_data[i].type;
        strcpy(all_forms[forms_n][i].data, all_data[i].data);
    }
}

void add_new_form(int max_y, int max_x, component *components, int components_n, WINDOW *command, WINDOW* command_border, form_data *all_data, form_data **all_forms, int forms_n){

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
        int component_idx;
        int end;
        end = select_component(components_n, components_win, properties, components, &component_idx, all_data);
        clear_window(components_win, "");
        if (!end){
            get_add_data(command, (&all_data[component_idx]) );
        } else if (end == -1){
            save_form_data(all_data, all_forms, components_n, forms_n);
            break;
        }
        else{
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

void save_forms_to_file(form_data **all_forms, char* file_name, int forms_n, int components_n){

    char file_address[116];

    struct stat st = {0};

    if (stat("FormsData", &st) == -1) {
        mkdir("FormsData", 0700);
    }

    sprintf(file_address, "FormsData/%s", file_name);

    FILE *fp;
    fp = fopen(file_address, "wb+");

    int n = all_forms[0][0].type;

    form_data file_data_container;
    file_data_container.type = forms_n;

    fwrite(&file_data_container, sizeof(form_data), 1, fp);
    for (int i = 0; i < forms_n; ++i) {
        fwrite(all_forms[i], sizeof(form_data)*components_n, 1, fp);
    }
    fclose(fp);

}

void manage(int max_y, int max_x){

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

    // Open file

    while (1){

        // get file name from user

        curs_set(1);
        mvwprintw(command, 0, 0, "Please enter form's name : ");
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

    mvprintw(0,0, "%s", file_name);
    refresh();


    // Open data file
    char file_address[116];
    sprintf(file_address, "FormsData/%s", file_name);
    form_data file_data_container;
    form_data **all_forms = malloc(1000*sizeof(form_data*));
    int forms_n;

    FILE *data_file;
    data_file = fopen(file_address, "rb+");

    if (!data_file){
        data_file = fopen(file_address, "wb+");
        forms_n = 0;
        for (int i = 0; i < 1000; ++i) {
            all_forms[i] = malloc(sizeof(form_data)*(components_n+1));
        }
    } else {
        fread(&file_data_container, sizeof(form_data), 1, data_file);
        forms_n = file_data_container.type;
        for (int i = 0; i < forms_n; ++i) {
            all_forms[i] = malloc(sizeof(form_data)*(components_n+1));
            fread(all_forms[i], sizeof(form_data)*(components_n+1), 1, data_file);
        }
    }

    int choice = 0;

    while (choice != 2) {


        clear_window(command, "Select one of these commands.");
        char choices[3][7] = {"|Add|", "|Find|", "|Exit|"};

        choice = get_choice(max_x, 7, choices, command, 3, 24, 1, 0);

        if (choice == 0){
            form_data *all_data = malloc((components_n+1)*sizeof(form_data));

            clear_window(command, "Press enter on a component to add data or press backspace to discard or Ctrl + s to save form.");
            cbreak();
            add_new_form(max_y, max_x, components, components_n, command, command_border, all_data, all_forms, forms_n);
            forms_n++;
//            cbreak();
        } else if (choice == 2){
            clear_window(command, "Do you want to save changes?");
            char choices_exit[2][5] = {"YES", "NO"};
            int save = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 1, 0);
            if (!save)
                save_forms_to_file(all_forms, file_name, forms_n, components_n);
        }
    }
}