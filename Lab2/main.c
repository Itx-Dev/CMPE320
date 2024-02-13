#include "modes.h"

int main(int argc, char* args[]) {
    if (argc > 1) {
        batchMode(argc, args);
    } else if (argc <= 1) {
        interactiveMode(argc, args);
    }
}