#include "s21_cat.h"

int main(int argc, char *argv[]) {
  Params p;
  char **files;
  int fcount = 0;
  get_params(&p, argc, argv, &fcount, &files);
  if (p.error == 0) go(p, fcount, files);
  for (int i = 0; i < fcount; i++) free(files[i]);
  free(files);
  return 0;
}
int read_str(char *str, FILE *fp) {
  int i = 0;
  while (1) {
    str[i] = fgetc(fp);
    if (feof(fp) != 0) break;
    i++;
    if (str[i - 1] == '\n') break;
  }
  str[i] = '\0';
  if (i == 0) i = -1;
  return i;
}
int get_params(Params *p, int argc, char **argv, int *fcount, char ***files) {
  p->error = 0;
  p->b = 0;
  p->e = 0;
  p->n = 0;
  p->s = 0;
  p->t = 0;
  p->v = 0;
  char short_opt[] = "beEnstTv";

  static struct option long_opt[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  int c;
  while ((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != EOF) {
    switch (c) {
      case 'b':
        p->b = 1;
        break;
      case 'e':
        p->e = 1;
        p->v = 1;
        break;
      case 'E':
        p->e = 1;
        break;
      case 'n':
        p->n = 1;
        break;
      case 's':
        p->s = 1;
        break;
      case 't':
        p->t = 1;
        p->v = 1;
        break;
      case 'T':
        p->t = 1;
        break;
      case 'v':
        p->v = 1;
        break;
      case '?':
        p->error++;
    }
  }
  if (optind < argc) {
    *fcount = argc - optind;
    *files = malloc((argc - optind) * sizeof(char *));
    int i = 0;
    do {
      (*files)[i] = malloc((strlen(argv[optind]) + 1) * sizeof(char));
      strcpy((*files)[i], argv[optind]);
      i++;
    } while (++optind < argc);
  } else {
    *fcount = 0;
  }
  return 0;
}

int go(Params p, int fcount, char **files) {
  FILE *fp;
  int bufsize = 4096;
  char buffer[bufsize];
  int line = 0, lastempty = 0, lastendsnewstr = 1;

  for (int i = 0; i < fcount; i++) {
    fp = fopen(files[i], "r");
    if (fp == NULL) {
      printf("s21_cat: %s: No such file or directory\n", files[i]);
    } else {
      int count;
      while ((count = read_str(buffer, fp)) != -1) {
        int empty = 0;
        if (strcmp(buffer, "\n") == 0) empty = 1;
        if (p.t == 1) {
          for (int i = 0; i < count; i++) {
            if (buffer[i] == '\t') {
              for (int j = count + 1; j > i + 1; j--) buffer[j] = buffer[j - 1];
              buffer[i] = '^';
              buffer[i + 1] = 'I';
              count++;
            }
          }
        }
        if ((p.e == 1) && (buffer[count - 1] == '\n')) {
          buffer[count - 1] = '$';
          buffer[count] = '\n';
          buffer[count + 1] = 0;
          count++;
        }
        if (p.v == 1) {
          for (int i = 0; i < count; i++) {
            if ((buffer[i] >= 0) && (buffer[i] < 32) && (buffer[i] != 9) &&
                (buffer[i] != 10)) {
              for (int j = count + 1; j > i + 1; j--) buffer[j] = buffer[j - 1];
              buffer[i + 1] = buffer[i] + 64;
              buffer[i] = '^';
              count++;
            }
            if (buffer[i] == 127) {
              for (int j = count + 1; j > i + 1; j--) buffer[j] = buffer[j - 1];
              buffer[i + 1] = '?';
              buffer[i] = '^';
              count++;
            }
            if (((unsigned char)buffer[i] >= 128) &&
                ((unsigned char)buffer[i] < 160)) {
              for (int j = count + 3; j > i + 1; j--) buffer[j] = buffer[j - 3];
              buffer[i + 3] = buffer[i] - 64;
              buffer[i + 2] = '^';
              buffer[i + 1] = '-';
              buffer[i] = 'M';
              count = count + 3;
            }
#ifndef __APPLE__
            if ((unsigned char)buffer[i] >= 160) {
              for (int j = count + 2; j > i + 1; j--) buffer[j] = buffer[j - 2];
              buffer[i + 2] = buffer[i] - 128;
              buffer[i + 1] = '-';
              buffer[i] = 'M';
              count = count + 2;
            }
#endif
          }
        }
        print_str(p, &lastempty, &line, &lastendsnewstr, buffer, count, empty);
      }
      fclose(fp);
#ifdef __APPLE__
      line = 0;
#endif
    }
  }
  return 0;
}

int print_str(Params p, int *lastempty, int *line, int *lastendsnewstr,
              char *str, int count, int empty) {
  if (!((p.s == 1) && (*lastempty == 1) && (empty == 1))) {
#ifndef __APPLE__
    if (*lastendsnewstr == 1) {
#endif
      if (((p.n == 1) && (p.b == 0)) || ((p.b == 1) && (empty == 0))) {
        (*line)++;
        printf("%6d\t", *line);
      }
#ifndef __APPLE__
    }
#endif
    for (int i = 0; i < count; i++) {
      fputc(str[i], stdout);
    }

    if (str[count - 1] == '\n') {
      *lastendsnewstr = 1;
    } else {
      *lastendsnewstr = 0;
    }
    if (empty == 1) {
      *lastempty = 1;
    } else {
      *lastempty = 0;
    }
  }
  return 0;
}