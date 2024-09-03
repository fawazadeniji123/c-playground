#include <stdio.h>

int main() {
    const int n = 1;
    int *pN = (int *)&n; // Casting away const-ness (not recommended)
    *pN = 0; // Undefined behavior: attempting to modify a constant variable

    printf("n = %d\n", n);
    return 0;
}
