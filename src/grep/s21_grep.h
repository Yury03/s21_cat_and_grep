#ifndef SRC_GREP_S21_GREP_H
#define SRC_GREP_S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int e;
  int v;
  int i;
  int l;
  int c;
  int n;
  int h;
  int s;
  int f;
  int o;
} Grep_flags;

regex_t *Compile_re(regex_t *myreg, char **patterns, int lines_count,
                    int cflags, int *regerr);
char **addPattern(char **patterns, char *new_line, int *lines_count, int *fl);
void addPattern_from_file(char ***patterns, char *file_name, int *lines_count,
                          int *fl);
int parser(int argc, char *argv[], Grep_flags *flags, char ***patterns,
           int *lines_count);
void printLine(Grep_flags *flags, char *line, int *str_count);
void printLineWithName(char *name, Grep_flags *flags, char *line,
                       int *str_count);
void In_opt_print(int argc, char *name, Grep_flags *flags, int matches_count,
                  int l_point);
void Inner_opt(int argc, char *name, Grep_flags *flags, char *line,
               int *str_count, int *matches_count, int *l_point);
void Do_options(int argc, char *argv[], regex_t **myreg, char **patterns,
                int lines_count, Grep_flags *flags);
void freeArrays(char **patterns, regex_t *myreg, int lines_count);

#endif  // SRC_GREP_S21_GREP_H
