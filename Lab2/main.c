#include "processShell.h"
#include "error.h"
#include <stdio.h>


int main(int argc, char* args[]) {
    FILE *fp;

    if (argc > 1) {
        fp = fopen(args[1], "r");
    } else if (argc <= 1) {
        fp = stdin;
    } else {
        throwError();
    }

    processShell(fp);

}