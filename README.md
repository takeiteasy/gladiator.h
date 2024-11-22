# gladiator

Very simple cross-platform (windows, mac, linux) arena implementation.

```
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

> clang test.c -o test
> ./test
1
2
3
4
5
6
```

## LICENSE

```
Very simple cross-platform arena implementation

Copyright (C) <year> George Watson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
