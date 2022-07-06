#include <ncurses.h>
#include <string.h>
#include <json-c/json.h>
#include "header.h"
#include <sys/stat.h>
#include <sys/types.h>
#define whiteking 0x2654

int max_y, max_x;
FILE *fp;
char file_address[116];

typedef struct {
    int x_pos;
    int y_pos;
    int width;
    int height;
    int checked;
    char label_text[30];
} component;

component* components;
int components_n = 0;


void clear_window(WINDOW *win, char text[]){
    wclear(win);
    wattron(win, A_BOLD);
    mvwprintw(win, 0, 0, "%s", text);
    wattroff(win, A_BOLD);
    wrefresh(win);
}

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


    strcat(file_name, ".json");
    sprintf(file_address, "FormsTemplates/%s", file_name);


    fp = fopen(file_address, "r+");

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
            strcat(file_name, ".json");
            sprintf(file_address, "FormsTemplates/%s", file_name);
            fp = fopen(file_address, "r+");
        }

    }

    fp = fopen(file_address, "w+");
    fclose(fp);

    curs_set(0);

}

void rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}


void put_component_on_view(int type, component created_component, WINDOW *win){
    WINDOW *container_win;
    if (type == 0) {
        container_win = derwin(win, created_component.height+2,
                                       created_component.width+2+(int)strlen(created_component.label_text),
                                       created_component.y_pos,
                                       created_component.x_pos);
        mvwprintw(container_win, (created_component.height+2)/2, 0, "%s", created_component.label_text);
        WINDOW *txt_bx = derwin(container_win, created_component.height+2, created_component.width+2, created_component.y_pos, (int)strlen(created_component.label_text));
        box(txt_bx, 0, 0);
        touchwin(container_win);
        touchwin(win);
        wrefresh(win);
        wrefresh(txt_bx);
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
                               created_component.y_pos);
        mvwprintw(container_win, 0, 0, "%s", created_component.label_text);
        touchwin(win);
        wrefresh(container_win);
    }
}

void process_option(int option, WINDOW *command, WINDOW *view, char choices[][13]){

    components = malloc(100 * sizeof(component));

    int confirmed_component = 0;
    component created_component;

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

        } else {

            // Button, Label

            created_component.checked = -1;
            created_component.height = -1;
            created_component.width = -1;

            mvwprintw(command, 0, 0, "label : %s | x_pos : %d | y_pos : %d", created_component.label_text, created_component.x_pos, created_component.y_pos);
        }

        mvwprintw(command, 1, 0, "Do you want to create this component?\n ");
        char choices_exit[2][5] = {"YES", "NO"};
        confirmed_component = !( get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 2, 1) );


    }

    put_component_on_view(option, created_component, view);

    if (option == 4) {
        mvwprintw(command, 1, 0, "|Do you want to save created form?\n ");
        char choices_exit[2][5] = {"YES", "NO"};
        int save;
        save = get_choice(max_x, 5, choices_exit, command, 2, max_x-7, 2, 1);
        if (!save){
//            save_file();
        } else {
//            delete_file();
        }
    }
    wrefresh(command);
//    switch (option) {
//        case 0:
//
//    }
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

    char choices[5][13] = {"|TextBox|", "|Button|", "|Check Box|", "|Label|", "|Exit|"};

    mvwprintw(command, 0, 0, "Please choose one of the options to add on form , otherwise choose exit or help.");

    int option;
    option = get_choice(max_x, 13, choices, command, 5, 48, 2, 0);

    wclear(command);
    process_option(option, command, view, choices);
}