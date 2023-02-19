#include "s21_grep.h"

regex_t *Compile_re(regex_t *myreg, char **patterns, int lines_count,
                    int cflags, int *regerr) {
  myreg = (regex_t *)malloc(lines_count * sizeof(regex_t));
  if (myreg) {
    for (int i = 0; i < lines_count; i++) {
      *regerr = regcomp(myreg + i, patterns[i], cflags);
    }
  } else {
    *regerr = -1;
  }
  return myreg;
}

char **addPattern(char **patterns, char *new_line, int *lines_count, int *fl) {
  char **temp = (char **)realloc(patterns, (*lines_count + 1) * sizeof(char *));
  if (temp) {
    patterns = temp;
    patterns[*lines_count] = strdup(new_line);
    if (patterns[*lines_count]) {
      *lines_count = *lines_count + 1;
    } else {
      *fl = -1;
    }
  } else {
    *fl = -1;
  }
  return patterns;
}

void addPattern_from_file(char ***patterns, char *file_name, int *lines_count,
                          int *fl) {
  FILE *file = fopen(file_name, "r");
  char str[1000];

  if (file != NULL) {
    while (fgets(str, 1000, file) != NULL) {
      if (index(str, '\n')) *(index(str, '\n')) = '\0';
      *patterns = addPattern(*patterns, str, lines_count, fl);
    }
    fclose(file);
  } else {
    *fl = -1;
  }
}

int parser(int argc, char *argv[], Grep_flags *flags, char ***patterns,
           int *lines_count) {
  int flag;
  int err = 0;

  while ((flag = getopt_long(argc, argv, "e:ivcnlhsof:", 0, NULL)) != -1) {
    switch (flag) {
      case 'e':
        flags->e = 1;
        *patterns = addPattern(*patterns, optarg, lines_count, &err);
        if (err == -1) {
          fprintf(stderr, "Memory allocation error");
        }
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f':
        flags->f = 1;
        addPattern_from_file(patterns, optarg, lines_count, &err);
        if (err == -1) {
          fprintf(stderr, "%s: %s: No such file or directory", argv[0], optarg);
        }
        break;
      case 'o':
        flags->o = 1;
        break;
      default:
        fprintf(
            stderr,
            "usage: grep [-abcDEregerrHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
            "[-C[num]]\n\t[-e pattern] [-f file] [--binary-files=value] "
            "[--color=when]\n\t[--context[=num]] [--directories=action] "
            "[--label] [--line-buffered]\n\t[--null] [pattern] [file ...]");
        err = -1;
    }
  }
  return err;
}

void Print_line(Grep_flags *flags, char *line, int *str_count) {
  if (flags->n) {
    printf("%d:%s\n", *str_count, line);
  } else {
    printf("%s\n", line);
  }
}

void printLineWithName(char *name, Grep_flags *flags, char *line,
                       int *str_count) {
  if (flags->n) {
    printf("%s:%d:%s\n", name, *str_count, line);
  } else {
    printf("%s:%s\n", name, line);
  }
}

void Inner_opt(int argc, char *name, Grep_flags *flags, char *line,
               int *str_count, int *matches_count, int *l_point) {
  if ((!flags->c && !flags->l && !flags->o) ||
      (!flags->c && !flags->l && flags->o && flags->v)) {
    if ((argc - optind == 1) || flags->h) {
      Print_line(flags, line, str_count);
    } else {
      printLineWithName(name, flags, line, str_count);
    }
  } else if (flags->c && flags->l) {
    *matches_count = 1;
    *l_point = 1;
  } else if (flags->c && !flags->l) {
    *matches_count = *matches_count + 1;
  } else if (!flags->c && flags->l) {
    *l_point = 1;
  }
}

void In_opt_print(int argc, char *name, Grep_flags *flags, int matches_count,
                  int l_point) {
  if ((argc - optind == 1) || flags->h) {
    if (flags->c && !flags->l) {
      printf("%d\n", matches_count);
    } else if (flags->c && flags->l) {
      if (l_point) {
        printf("%d\n", matches_count);
        printf("%s\n", name);
      } else {
        printf("%d\n", matches_count);
      }
    } else if (!flags->c && flags->l && l_point) {
      printf("%s\n", name);
    }
  } else {
    if (flags->c && !flags->l) {
      printf("%s:%d\n", name, matches_count);
    } else if (flags->c && flags->l) {
      if (l_point) {
        printf("%s:%d\n", name, matches_count);
        printf("%s\n", name);
      } else {
        printf("%s:%d\n", name, matches_count);
      }
    } else if (!flags->c && flags->l && l_point) {
      printf("%s\n", name);
    }
  }
}

void Do_options(int argc, char *argv[], regex_t **myreg, char **patterns,
                int lines_count, Grep_flags *flags) {
  char line[1000];
  int flag, error = 0, matches_count, str_count, l_point, offset = 0;
  FILE *f;
  regmatch_t match;
  int regerr = 0;

  if (!flags->i)
    *myreg = Compile_re(*myreg, patterns, lines_count, REG_EXTENDED, &regerr);
  else
    *myreg = Compile_re(*myreg, patterns, lines_count, REG_ICASE | REG_EXTENDED,
                        &regerr);
  if (regerr == 0) {
    for (int i = optind; i < argc; i++) {
      f = fopen(argv[i], "r");
      if (f != NULL) {
        l_point = 0;
        str_count = 0;
        matches_count = 0;
        while (fgets(line, 1000, f) != NULL) {
          flag = 0;
          offset = 0;
          if (index(line, '\n')) *(index(line, '\n')) = '\0';
          if (flags->v) {
            for (int j = 0; j < lines_count; j++) {
              if ((error = regexec(*myreg + j, line, 1, &match, 0)) == 0) {
                flag = 1;
                break;
              }
            }
            str_count++;
            if (!flag) {
              Inner_opt(argc, argv[i], flags, line, &str_count, &matches_count,
                        &l_point);
            }
          } else {
            str_count++;
            int j = 0;
            while (j < lines_count) {
              if ((error = regexec(*myreg + j, line, 1, &match, 0)) == 0) {
                flag = 1;
                if (flags->o) {
                  while (((error = regexec(*myreg + j, line + offset, 1, &match,
                                           0)) == 0)) {
                    if (!flags->l && !flags->c && !flags->v) {
                      if (((argc - optind > 1) && !flags->h) && (offset == 0)) {
                        printf("%s:", argv[i]);
                      }
                      if (flags->n && (offset == 0)) {
                        printf("%d:", str_count);
                      }
                      printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                             (line + offset + match.rm_so));
                    }
                    offset += match.rm_eo;
                  }
                }
              }
              j++;
            }
            if (flag) {
              Inner_opt(argc, argv[i], flags, line, &str_count, &matches_count,
                        &l_point);
            }
          }
        }
        In_opt_print(argc, argv[i], flags, matches_count, l_point);
        fclose(f);
      } else if (!flags->s) {
        fprintf(stderr, "%s: %s: No such file or directory\n", argv[0],
                argv[i]);
      }
    }
  } else {
    printf("Regex error");
  }
}

void freeArrays(char **patterns, regex_t *myreg, int lines_count) {
  if (patterns != NULL) {
    for (int i = 0; i < lines_count; i++) {
      free(patterns[i]);
    }
    free(patterns);
  }
  if (myreg != NULL) {
    for (int i = 0; i < lines_count; i++) {
      regfree(myreg + i);
    }
    free(myreg);
  }
}

int main(int argc, char *argv[]) {
  Grep_flags flags = {0};
  char **patterns = NULL;
  int err = 0;
  int lines_count = 0;
  int fl = 0;

  regex_t *myreg = NULL;

  if ((err = parser(argc, argv, &flags, &patterns, &lines_count)) == 0) {
    if ((!flags.e) && (!flags.f) && (argc != optind)) {
      patterns = addPattern(patterns, argv[optind], &lines_count, &fl);
      optind++;
    }
    if (argc == optind) {
      fprintf(
          stderr,
          "usage: grep [-abcDEregerrHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
          "[-C[num]]\n\t[-e pattern] [-f file] [--binary-files=value] "
          "[--color=when]\n\t[--context[=num]] [--directories=action] "
          "[--label] [--line-buffered]\n\t[--null] [pattern] [file ...]");
    } else {
      Do_options(argc, argv, &myreg, patterns, lines_count, &flags);
    }
  }

  freeArrays(patterns, myreg, lines_count);

  return 0;
}
