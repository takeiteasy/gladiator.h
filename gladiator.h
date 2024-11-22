/* gladiator.h -- https://github.com/takeiteasy/gladiator

 gladiator -- Cross-platform arena memory storage pool
 
 Copyright (C) 2024  George Watson
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#ifndef GLADIATOR_HEADER
#define GLADIATOR_HEADER
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#define ARENA_PLATFORM_WINDOWS
#endif

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#ifdef ARENA_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

typedef struct arena_region {
    void* memory;
    size_t size_of_memory;
    struct arena_region *next, *prev;
} arena_region_t;

typedef struct arena {
    arena_region_t *head, *tail;
} arena_t;

void gladiator_reset(arena_t *arena, void(*callback)(void*));
void* gladiator_alloc(arena_t *arena, size_t size_of_memory);
void gladiator_free(arena_t *arena, void *memory);

#if defined(__cplusplus)
}
#endif
#endif // GLADIATOR_HEADER

#if defined(GLADIATOR_IMPLEMENTATION) || defined(EZ_IMPLEMENTATION)
#if defined(_WIN32) || defined(_WIN64)
#define GLADIATOR_PLATFORM_WINDOWS
#endif

static void* mem_alloc(size_t size) {
#if defined(GLADIATOR_PLATFORM_WINDOWS)
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
#else
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}

static int mem_free(void *location, size_t size_of_memory) {
#if defined(GLADIATOR_PLATFORM_WINDOWS)
    return VirtualFree(location, size_of_memory, MEM_RELEASE);
#else
    return munmap(location, size_of_memory) != -1;
#endif
}

void gladiator_reset(arena_t *arena, void(*callback)(void*)) {
    arena_region_t *current = arena->head;
    while(current) {
        arena_region_t *next = current->next;
        if (callback)
            callback(current->memory);
        assert(mem_free(current->memory, current->size_of_memory));
        assert(mem_free(current, sizeof(arena_region_t)));
        current = next;
    }
    arena->head = NULL;
    arena->tail = NULL;
}

void* gladiator_alloc(arena_t *arena, size_t size_of_memory) {
    arena_region_t *region = (arena_region_t*)mem_alloc(sizeof(arena_region_t));
    region->memory = mem_alloc(size_of_memory);
    region->size_of_memory = size_of_memory;

    if (!arena->tail) {
        region->prev = NULL;
        region->next = NULL;
        arena->head = arena->tail = region;
    } else {
        region->prev = arena->tail;
        arena->tail->next = region;
        arena->tail = region;
    }

    return region->memory;
}

void gladiator_free(arena_t *arena, void *memory) {
    arena_region_t *cursor = arena->head;
    while (cursor) {
        if (cursor->memory == memory) {
            if (cursor->prev && cursor->prev->next)
                cursor->prev->next = cursor->next;
            if (cursor->next && cursor->next->prev)
                cursor->next->prev = cursor->prev;
            assert(mem_free(cursor->memory, cursor->size_of_memory));
            assert(mem_free(cursor, sizeof(arena_region_t)));
            return;
        }
        cursor = cursor->next;
    }
    assert(0); // Not found
}
#endif
