#include <stdio.h>
#include <unistd.h>

int main() {
    execl("./sequental_min_max", "sequental_min_max", "123", "40", (char *)NULL);

    perror("exec failed");
    return 1;
}