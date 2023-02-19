#ifndef SRC_CAT_MAIN_H_
#define SRC_CAT_MAIN_H_
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  int error, b, e, n, s, t, v;
} Params;
int read_str(char *str, FILE *fp);
int get_params(Params *p, int argc, char **argv, int *fcount, char ***files);
int go(Params p, int fcount, char **files);
int print_str(Params p, int *lastempty, int *line, int *lastendsnewstr,
              char *str, int count, int empty);
#endif  // SRC_CAT_MAIN_H_