#include <ncurses.h>
#include <string.h>
#include <json-c/json.h>
#include "header.h"
#include <sys/stat.h>
#include <unistd.h>


int max_y, max_x;
int view_max_x, view_max_y;
FILE *fp;
char file_address[116];

component* components;
int components_n = 0;
int n_point = 0;


void make_new_file(WINDOW *win){
    char file_name[100];
    curs_set(1);
    echo();
    mvwprintw(win, 0, 0, "Please enter the name of form (maximum 100 character) : ");
    wgetstr(win, file_name);
    struct stat st = {0};

    if (stat("FormsTemplates", &st) == -1) {
        mkdir("FormsTemplates", 0700);
    }


    strcat(file_name, ".txt");
    sprintf(file_address, "FormsTemplates/%s", file_name);


    fp = fopen(file_address, "rb+");

    int not_overwrite = 1;
    while (fp && not_overwrite){
        clear_window(win, "");
        mvwprintw(win, 0, 0, "A file with this name already exists;\nDo you want to overwrite it?");
        char choices_exit[2][5] = {"YES", "NO"};
        not_overwrite = get_choice(max_x, 5, choices_exit, win, 2, max_x-7, 2, 0);

        if (not_overwrite){
            clear_window(win, "");
            curs_set(1);
            echo();
            mvwprintw(win, 0, 0, "Please enter the name of form (maximum 100 character) : ");
            wgetstr(win, file_name);
            strcat(file_name, ".txt");
            sprintf(file_address, "FormsTemplates/%s", file_name);
            fp = fopen(file_address, "rb+");
        }

    }

    fp = fopen(file_address, "wb+");

    curs_set(0);

}


void put_component_on_view(int type, component created_component, WINDOW *win){
    WINDOW *container_win;
    if (type == 0) {
        container_win = derwin(win, created_component.height+2,  created_component.width+2, created_component.y_pos, created_component.x_pos);
        box(container_win, 0, 0);
        mvwprintw(container_win, 0, 0, "%s:", created_component.label_text);
        touchwin(win);
        wrefresh(win);
        wrefresh(container_win);
    } else if (type == 1) {
        container_win = derwin(win, 3,
                               (int)strlen(created_component.label_text)+2,
                               created_component.y_pos,
                               created_component.x_pos);
        mvwprintw(container_win, 1, 1, "%s", created_component.label_text);
        box(container_win, 0, 0);
        touchwin(win);
        wrefresh(container_win);
    } else if (type == 2) {
        container_win = derwin(win, 3,
                               (int) strlen(created_component.label_text)+5,
                               created_component.y_pos,
                               created_component.x_pos);
        mvwprintw(container_win, 1, 0, "%s", created_component.label_text);
        WINDOW *check = derwin(container_win, 3,
                               5, 0 , (int)strlen(created_component.label_text));
        if (created_component.checked == 0){
            mvwprintw(check, 1, 2, "X");
        }

        box(check, 0, 0);
        touchwin(win);
        wrefresh(container_win);
    } else if (type == 3) {
        container_win = derwin(win, 1, (int) strlen(created_component.label_text),
                               created_component.y_pos,
                               created_component.x_pos);
        mvwprintw(container_win, 0, 0, "%s", created_component.label_text);
        touchwin(win);
        wrefresh(container_win);
    }
}

int process_option(int option, WINDOW *command, WINDOW *view, char choices[][13], int points[][2]){

    int confirmed_component = 0;
    component created_component;

    int total_height;
    int total_width;

    while (!confirmed_component && option != 4){
        clear_window(command, choices[option]);

        echo();

        mvwprintw(command, 1, 0, "|Label text (maximum 20 characters) : ");
        curs_set(1);
        wgetstr(command, created_component.label_text);
        clear_window(command, choices[option]);
        mvwprintw(command, 1, 0, "|X Position (only number and maximum %d) : ", max_x);
        wscanw(command, "%d", &created_component.x_pos);
        clear_window(command, choices[option]);
        mvwprintw(command, 1, 0, "|Y Position (only number and maximum %d) : ", max_y);
        wscanw(command, "%d", &created_component.y_pos);
        clear_window(command, choices[option]);
        if (option == 0){

            // TextBox

            created_component.checked = -1;

            mvwprintw(command, 1, 0, "|Width & Height of textbox (only numbers and maximums are %d & %d) : ", max_x - (int)strlen(created_component.label_text), max_y-9);
            wscanw(command, "%d %d", &created_component.width, &created_component.height);
            clear_window(command, choices[option]);
            mvwprintw(command, 0, 0, "label : %s | x_pos : %d | y_pos : %d | Width : %d | Height : %d", created_component.label_text, created_component.x_pos, created_component.y_pos, created_component.width, created_component.height);

            total_height = created_component.height+2;
            total_width = created_component.width+2;

        }
        else if (option == 2){

            // CheckBox

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


        for (int i = 0; i < n_point; ++i) {
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
            mvwprintw(command, 1, 0, "Do you want to create this component?\n ");
            char choices_exit[2][5] = {"YES", "NO"};
            confirmed_component = !( get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 2, 1) );
            for (int i = 0; i < c_p_n; ++i) {
                points[n_point][0] = component_points[i][0];
                points[n_point][1] = component_points[i][1];
                n_point++;
            }
        }

    }

    // Print component on view window
    put_component_on_view(option, created_component, view);


    // Add component to the array
    if (option != 4){
        components[components_n] = created_component;
        components_n++;
    }


    wrefresh(command);

    if (option == 4) {
        mvwprintw(command, 1, 0, "|Do you want to save created form?\n ");
        char choices_exit[2][5] = {"YES", "NO"};
        int save;
        save = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 2, 1);
        if (!save){
            save_file(components, components_n, fp);
        } else {
//            delete_file();
        }
        return 1;
    }
}

void create(){
    noecho();

    getmaxyx(stdscr, max_y, max_x);

    move( 1, (max_x/2)-7 );

    attron(A_BOLD);

    printw("Create New Form");
    mvwprintw(stdscr, 2, 0, "View : ");
    mvwprintw(stdscr, max_y-6, 0, "Command : ");

    attroff(A_BOLD);

    refresh();

    WINDOW *view = newwin(max_y-11, max_x-2, 4, 1);
    WINDOW *view_border = newwin(max_y-9, max_x, 3, 0);
    WINDOW *command = newwin(3, max_x-2, max_y-4, 1);
    WINDOW *command_border = newwin(5, max_x, max_y-5, 0);

    box(view_border, '|', '-');
    box(command_border, '|', '-');

    wrefresh(view_border);
    wrefresh(view);
    wrefresh(command_border);
    wrefresh(command);

    make_new_file(command);
    clear_window(command, "\0");

    getmaxyx(view, view_max_y, view_max_x);
    int points[view_max_x*view_max_y][2];

    int end = 0;
    char choices[5][13] = {"|TextBox|", "|Button|", "|Check Box|", "|Label|", "|Exit|"};

    components = malloc(100 * sizeof(component));

    while (!end){
        clear_window(command, "\0");

        mvwprintw(command, 0, 0, "Please choose one of the options to add on form , otherwise choose exit or help.");

        int option;
        option = get_choice(max_x, 13, choices, command, 5, 48, 2, 0);

        wclear(command);
        end = process_option(option, command, view, choices, points);

    }

}