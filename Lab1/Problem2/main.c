#include <stdio.h>
#include "bar.h"
#include "foo.h"

int x = 0;

int main() {
    foo();
    bar();
    printf("x is %d\n", x);
}