#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path, size_t *out_size) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }

  long size = ftell(f);
  if (size < 0) {
    fclose(f);
    return NULL;
  }
  rewind(f);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    fclose(f);
    return NULL;
  }

  size_t read = fread(buffer, 1, size, f);
  fclose(f);

  if (read != (size_t)size) {
    free(buffer);
    return NULL;
  }

  buffer[size] = '\0';

  if (out_size)
    *out_size = size;
  return buffer;
}
