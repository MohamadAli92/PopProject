#include "header.h"

void save_file(component* components, int components_n, FILE* fp){
    fwrite(components, sizeof(component), components_n*sizeof(component), fp);
    fclose(fp);
}