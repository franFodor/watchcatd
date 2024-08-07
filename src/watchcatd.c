#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_ARGS 1
#define ERR_MEM_ALLOC 2

int main(int argc, char **argv) {
  char *base_path = NULL;
  char *token = NULL;

  if (argc != 2) {
    fprintf(stderr, "USAGE: watchcatd <PATH>");
    exit(ERR_ARGS);
  }

  base_path = (char *)malloc(sizeof(char) * strlen(argv[1]) + 1);
  if (!base_path) {
    fprintf(stderr, "Memory allocation failed!");
    exit(ERR_MEM_ALLOC);
  }
  strcpy(base_path, argv[1]);

  token = strtok(base_path, "/");
  while (token != NULL) {
    base_path = token;
    token = strtok(NULL, "/");
  }

  fprintf(stdout, "%s", base_path);

  return 0;
}
