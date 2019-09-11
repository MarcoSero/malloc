#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

/*
 * The allocated memory: 64 bytes.
 */
static uint8_t MEMORY_POOL[64];

/*
 * Describes the position and the size of a segment of free space in memory.
 */
struct free_entry {
  void *ptr;
  uint64_t size;
};
typedef struct free_entry free_entry_t;

/*
 * The list of free entries.
 * Initialized with only one entry that points to the start of MEMORY_POOL and spans its entire 
 * size.
 */
static free_entry_t FREE_LIST[64] = {
  (free_entry_t){
    .ptr = MEMORY_POOL,
    .size = 64,
  },
};
static uint64_t FREE_LIST_USED = 1;

/*
 * Helper function to print |FREE_LIST|. 
 */
void print_free_list() {
  printf("FREE_LIST:\n");
  for (uint64_t i = 0; i < FREE_LIST_USED; i++) {
    free_entry_t *entry;
    entry = &FREE_LIST[i];
    printf("  %p (%lld)\n", entry->ptr, entry->size);
  }
}

/*
 * Finds the 'best' entry for |size| in |FREE_LIST|.
 * The best is defined as the smallest one.
 */
free_entry_t *find_free_entry(size_t size) {
  free_entry_t *best_entry = NULL;
  for (uint64_t i = 0; i < FREE_LIST_USED; i++) {
    free_entry_t *entry;
    entry = &FREE_LIST[i];
    if (entry->size >= size) {
      if (best_entry == NULL || entry->size < best_entry->size) {
        best_entry = entry;
      }
    }
  }
  if (best_entry != NULL) {
    return best_entry;
  }
  abort();
}

/*
 * The malloc implementation.
 */
void *malloc(size_t size) {
  // Make space for the size pointer, 1 byte.
  // If size is 4, the memory will be occupied as follow:
  //
  // ================================
  // SSSSSSSSpppp
  //
  // Where S is the space reserved to store the number 4 (1 byte), and p is the allocated space.
  size += 8;

  free_entry_t *entry;
  entry = find_free_entry(size);

  void *base_ptr; // The pointer to the base of the free entry.
  uint64_t *size_ptr; // The pointer to the size of the free entry.
  void *user_ptr; // The pointer that will be returned to the user.

  base_ptr = entry->ptr;
  size_ptr = base_ptr;
  user_ptr = base_ptr + 8;
  // Set the value of the size pointer to the size. It will be used by free() to understand how
  // much memory can be freed.
  *size_ptr = size;

  entry->ptr += size;
  entry->size -= size;

  printf("malloc()\n");
  print_free_list();

  return user_ptr;
}

/*
 * The free implementation.
 */
void free(void *user_ptr) {
  free_entry_t *entry;
  entry = &FREE_LIST[FREE_LIST_USED];

  void *base_ptr;
  uint64_t *size_ptr;
  uint64_t size;

  base_ptr = user_ptr - 8;
  size_ptr = base_ptr;
  size = *size_ptr;

  entry->ptr = base_ptr;
  entry->size = size;

  FREE_LIST_USED++;

  printf("free()\n");
  print_free_list();
}

int main(int argc, const char* argv[]) {
  char *a;
  char *b;
  char *c;

  a = malloc(4);
  b = malloc(4);
  c = malloc(4);

  strcpy(a, "aaa\0");
  strcpy(b, "bbb\0");
  strcpy(c, "ccc\0");

  printf("%p\n", a);
  printf("%p\n", b);
  printf("%p\n", c);

  free(b);

  char *d;
  d = malloc(4);
  printf("%p\n", d);

  free(a);
  char *e;
  e = malloc(4);
  printf("%p\n", e);

  free(c);
  free(d);
  free(e);

  return 0;
}
