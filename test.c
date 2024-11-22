#include <stdio.h>
#define GLADIATOR_IMPLEMENTATION
#include "gladiator.h"

static int numbers[6] = { 1, 2, 3, 4, 5, 6 };

static void callback(void *memory) {
    int* number = (int*)memory;
    printf("%d\n", *number);
}

int main(int argc, const char *argv[]) {
    arena_t arena = {0};
    for (int i = 0; i < 6; i++) {
        int* number = gladiator_alloc(&arena, sizeof(int));
        *number = numbers[i];
    }
    gladiator_reset(&arena, callback);
    return 0;
}
