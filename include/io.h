#pragma once
#include <stdlib.h>
#include <stdio.h>

char * read_file(const char *path)
{
  FILE *f = fopen(path, "r");

  if (f) {
    // find out how large the file is
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    // allocate large enough string
    char *str = malloc(fsize + 1);

    if (str) {
      fread(str, 1, fsize, f);
      str[fsize] = 0;
    } else {
      fprintf(stderr, "read_file(%s): malloc null ptr.\n", path);
      return NULL;
    }

    fclose(f);
    return str;
  } else {
    fprintf(stderr, "read_file(%s): error opening file.\n", path);
    return NULL;
  }
}
