#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

struct line_t {
  char *data;
  size_t cap;
  ssize_t len;
  bool emitted;
};
typedef struct line_t Line;

char* initial_line_data() {
  char * mem = malloc(1);
  mem[0] = '\0';
  return mem;
}

int strncmp_shim(const char *str1, const char *str2, size_t num) {
  return strcmp(str1, str2);
}

struct flags_t {
  bool invert;
  bool use_prefix;
  size_t prefix_len;
};
typedef struct flags_t Flags;

static int parse_flags(int argc, char* argv[], Flags* flags) {
  int opt_ch;
  int opt_idx = 0;

  static struct option long_options[] = {
    {"help",      no_argument,       NULL, 'h'},
    {"invert",    no_argument,       NULL, 'i'},
    {"prefixlen", required_argument, NULL, 'p'},
    {0, 0, 0, 0}
  };

  while(1) {
    opt_ch = getopt_long(argc, argv, "ip:", long_options, &opt_idx);

    if (opt_ch == -1)
      break;

    switch (opt_ch) {
      case 'i':
        flags->invert = true;
        break;

      case 'p':
        flags->use_prefix = true;
        flags->prefix_len = atoi(optarg);
        break;

      case 'h':
      case ':':
      case '?':
        return 1;

      default:
        abort();
    }
  }

  return 0;
}

int main(int argc, char* argv[]) {
  struct flags_t flags = {false, false, 0};

  if (parse_flags(argc, argv, &flags)) {
    fprintf(stderr, "usage: runs [options]\n");
    fprintf(stderr, "  --[p]refixlen=LEN  bucket by LEN-byte prefix rather than whole line\n");
    fprintf(stderr, "  -i, --invert       emit orphans rather than runs\n");
    return 1;
  }

  int (*compare_lns)(const char*, const char*, size_t);
  compare_lns = flags.use_prefix ? strncmp : strncmp_shim;

  Line ln_a = (Line){initial_line_data(), 0, 0, false};
  Line ln_b = (Line){initial_line_data(), 0, 0, false};

  Line *ln_cur = &ln_a;
  Line *ln_prev = &ln_b;
  Line *ln_tmp;

  bool same_bucket;

  while ((ln_cur->len = getline(&(ln_cur->data), &(ln_cur->cap), stdin)) != -1) {
    ln_cur->emitted = false;

    if ((ln_cur->len - 1) < flags.prefix_len) {
      // skip empty lines
      if(ln_cur->data[0] == '\n') {
        continue;
      }

      fprintf(stderr, "undersized line: %s", ln_cur->data);
      exit(1);
    }

    same_bucket = (compare_lns(ln_prev->data, ln_cur->data, flags.prefix_len) == 0);

    if (same_bucket ^ flags.invert) {
      if(same_bucket && !ln_prev->emitted) {
        fwrite(ln_prev->data, 1, ln_prev->len, stdout);
      }
      fwrite(ln_cur->data, 1, ln_cur->len, stdout);
      ln_cur->emitted = true;
    }

    ln_tmp = ln_prev;
    ln_prev = ln_cur;
    ln_cur = ln_tmp;
  }

  free(ln_prev->data);
  free(ln_cur->data);
  return 0;
}
