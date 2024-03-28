#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "processShell.h"
#include "utilities.h"

int main(int argc, char *args[]) {
  FILE *fp;
  int rc = 0;

  if (argc == 2) {
    fp = fopen(args[1], "r");
    // File is not found
    if (fp == NULL) {
      throwError();
      exit(1);
    }
  }
  else if (argc <= 1) {
    fp = stdin;
  }
  else {
    throwError();
    exit(1);
  }
  processShell(fp);
  return 0;
}