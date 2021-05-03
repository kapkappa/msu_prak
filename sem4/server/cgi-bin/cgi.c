#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hi~!, ur args r : %s", getenv("QUERY_STRING"));
    return 0;
}
