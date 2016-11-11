#define MAP_SIZE 10
#define MAP_SIZE_E 11
#define S_VERTICAL 1
#define S_HORIZONTAL 0

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "logics.c"
#include "graphics.c"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    gtk_init(&argc, &argv);

    fillMap(); //fills map with .hiddenType
	countRowsCols(); //fills col- and rowcount
    makeUserMap(); //fills map with .userType
    makeOverlay();

    gtk_main();
    return 0;
}
