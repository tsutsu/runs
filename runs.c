#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>

struct line_t {
  char *data;
  size_t cap;
  ssize_t len;
  bool emitted;
};

enum mode_t {
  MODE_SINGLES = 0,
  MODE_RUNS = 1
};

typedef struct line_t Line;
typedef enum mode_t Mode;

char* initial_line_data() {
  char * mem = malloc(1);
  mem[0] = '\0';
  return mem;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <prefixlen>\n", argv[0]);
    exit(1);
  }

  char *prgname = basename(argv[0]);

  Mode mode = (strcmp(prgname, "singles") == 0) ? MODE_SINGLES : MODE_RUNS;

  size_t prefix_sz = atoi(argv[1]);

  Line ln_a = (Line){initial_line_data(), 0, 0, false};
  Line ln_b = (Line){initial_line_data(), 0, 0, false};

  Line *ln_cur = &ln_a;
  Line *ln_prev = &ln_b;
  Line *ln_tmp;

  bool same_bucket;

  while ((ln_cur->len = getline(&(ln_cur->data), &(ln_cur->cap), stdin)) != -1) {
    ln_cur->emitted = false;

    if ((ln_cur->len - 1) < prefix_sz) {
      // skip empty lines
      if(ln_cur->data[0] == '\n') {
        continue;
      }

      fprintf(stderr, "undersized line: %s", ln_cur->data);
      exit(1);
    }

    same_bucket = (strncmp(ln_prev->data, ln_cur->data, prefix_sz) == 0);

    if (mode == MODE_RUNS && same_bucket) {
      if(!ln_prev->emitted) {
        fwrite(ln_prev->data, 1, ln_prev->len, stdout);
      }
      fwrite(ln_cur->data, 1, ln_cur->len, stdout);
      ln_cur->emitted = true;
    } else if (mode == MODE_SINGLES && !same_bucket) {
      fwrite(ln_cur->data, 1, ln_cur->len, stdout);
    }

    ln_tmp = ln_prev;
    ln_prev = ln_cur;
    ln_cur = ln_tmp;
  }

  free(ln_prev->data);
  free(ln_cur->data);
  return 0;
}
