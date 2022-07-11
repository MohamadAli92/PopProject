#include <ncurses.h>

#ifndef POPPROJECT_HEADER_H
#define POPPROJECT_HEADER_H

typedef struct {
    int x_pos;
    int y_pos;
    int width;
    int height;
    int checked;
    char label_text[30];
} component;

int welcome_screen();
void create();
void edit(int max_y, int max_x);
void manage(int max_y, int max_x);
int get_choice(int max_x, size_t str_max_len, char choices[][str_max_len], WINDOW* win, int choices_n, int all_lens, int start_y, int start_x);
void save_file(component* components, int components_n, FILE* fp);
int open_file(char* file_name, component *components, int step);
void clear_window(WINDOW *win, char text[]);
void put_component_on_view(int option, component created_component, WINDOW *view);
int process_option(int option, WINDOW *command, WINDOW *view, char choices[][13], int points[][2]);

#endif //POPPROJECT_HEADER_H

