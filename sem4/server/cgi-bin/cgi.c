#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hi~!, ur args r : %s", getenv("params"));
    return 0;
}
