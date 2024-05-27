#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv) {
    assert(argc == 2);

    if (strcmp(argv[1], "-l") == 0) {

    } else {
        int errnum = atoi(argv[1]);
        printf("%d: %s\n", errnum, strerror(errnum));
    }
    
    return 0;
}