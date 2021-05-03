#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hi~!, Your args: %s", getenv("QUERY_STRING"));
    return 0;
}
