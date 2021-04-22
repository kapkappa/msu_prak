#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Content-type: text/html\n\n");
    printf("<html><body>");
    printf("Hi~!, ur args r : %s", getenv("params"));
    printf("</body></html>");
    return 0;
}
