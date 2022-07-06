#include <ncurses.h>

#ifndef POPPROJECT_HEADER_H
#define POPPROJECT_HEADER_H

int welcome_screen();
void create();
int get_choice(int max_x, size_t str_max_len, char choices[][str_max_len], WINDOW* win, int choices_n, int all_lens, int start_y, int start_x);

#endif //POPPROJECT_HEADER_H

