#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "processShell.h"
#include "error.h"


int main(int argc, char* args[]) {
    FILE *fp;
    int rc = 0;

    if (argc > 1) {
        fp = fopen(args[1], "r");
    } else if (argc <= 1) {
        fp = stdin;
    } else {
        throwError();
    }

    processShell(fp);

    return 0;
}