#include <errno.h>
#include <glob.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char *temp_file;
  char *pwm_file;
  int temp_wall;
} args_t;

static void parse_args(int argc, char *argv[], args_t *args) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      fprintf(stderr, "Invalid argument: %s\n", argv[i]);
      exit(1);
    }

    switch (argv[i][1]) {
    case 't':
      args->temp_file = argv[++i];
      break;
    case 'p':
      args->pwm_file = argv[++i];
      break;
    case 'w':
      args->temp_wall = atoi(argv[++i]);
      if (args->temp_wall < 0) {
        fprintf(stderr, "Invalid temperature: %s\n", argv[i]);
        exit(1);
      }
      break;
    case 'h':
      fprintf(stderr, "Usage: %s [-t temp_file] [-p pwm_file] [-w temp_wall]\n",
              argv[0]);
      exit(0);
      break;
    default:
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      break;
    }
  }
}

static FILE *open_file(const char *fname, const char *mode) {
  FILE *f = fopen(fname, mode);
  if (f == NULL) {
    fprintf(stderr, "Failed to open %s: %s\n", fname, strerror(errno));
    exit(1);
  }
  return f;
}

static int get_temp(const char *temp_file) {
  FILE *temp = open_file(temp_file, "r");
  char buf[10];
  fgets(buf, 10, temp);
  fclose(temp);
  return atoi(buf);
}

volatile sig_atomic_t stop = 0;

static inline void pwm_enable(FILE *pwm) {
  fputs("0\n", pwm);
  fflush(pwm);
}

static inline void pwm_disable(FILE *pwm) {
  fputs("2\n", pwm);
  fflush(pwm);
}

void main_loop(args_t *args) {
  FILE *pwm = open_file(args->pwm_file, "w");

  while (!stop) {
    int temp = get_temp(args->temp_file);
    if (temp > args->temp_wall) {
      pwm_enable(pwm);
    } else {
      pwm_disable(pwm);
    }
    sleep(1);
  }

  pwm_disable(pwm);
  fclose(pwm);
}

static void int_handler(__attribute__((unused)) int _) { stop = 1; }

char *glob_first(const char *pattern) {
  char *res = NULL;
  glob_t globbuf;

  switch (glob(pattern, 0, NULL, &globbuf)) {
  case 0:
    res = strdup(globbuf.gl_pathv[0]);
    fprintf(stderr, "Found: %s\n", res);
    break;
  case GLOB_NOMATCH:
    res = strdup(pattern);
    break;
  default:
    fprintf(stderr, "Failed to glob %s\n", pattern);
    exit(1);
  }

  globfree(&globbuf);
  return res;
}

void glob_args(args_t *args) {
  args->temp_file = glob_first(args->temp_file);
  args->pwm_file = glob_first(args->pwm_file);
}

void free_args(args_t *args) {
  free(args->temp_file);
  free(args->pwm_file);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, int_handler);
  args_t args = {
      "/sys/class/thermal/thermal_zone1/temp",
      "/sys/devices/platform/hp-wmi/hwmon/hwmon*/pwm1_enable",
      70000,
  };
  parse_args(argc, argv, &args);
  glob_args(&args);
  main_loop(&args);
  free_args(&args);
  return 0;
}
