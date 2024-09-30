// main.c
#include <stdio.h>
extern int add(int a, int b);

int main() {
    int a = 5;
    int b = 3;
    int result = add(a, b);

    printf("main.c: Result of %d + %d = %d\n", a, b, result);
    return 0;
}