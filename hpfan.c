#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")

struct argument {
  const char *temp_file;
  const char *pwm_file;
  int temp_wall;
};

static void parse_args(int argc, char *argv[], struct argument *args) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
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
            printf("Invalid temperature: %s\n", argv[i]);
            exit(1);
          }
          break;
        case 'h':
          printf("Usage: %s [-t temp_file] [-p pwm_file] [-w temp_wall]\n",
                 argv[0]);
          exit(0);
          break;
        default:
          printf("Unknown option: %s\n", argv[i]);
          break;
      }
    }
  }
}

static FILE *temp;
static FILE *pwm;

static int get_temp(const char *temp_file) {
  temp = fopen(temp_file, "r");
  if (temp == NULL) {
    printf("Failed to open %s: %s\n", temp_file, strerror(errno));
    exit(1);
  }
  char buf[10];
  fgets(buf, 10, temp);
  fclose(temp);
  temp = NULL;
  return atoi(buf);
}

void main_loop(const char *temp_file, const char *pwm_file, int temp_wall) {
  pwm = fopen(pwm_file, "w");
  if (pwm == NULL) {
    printf("Failed to open %s: %s\n", pwm_file, strerror(errno));
    exit(1);
  }
  while (1) {
    clear();
    int temp = get_temp(temp_file);
    printf("Temperature: %lf\n", temp / 1000.0);
    if (temp > temp_wall) {
      printf("Pwm enable: %d\n", 0);
      fprintf(pwm, "0\n");
    } else {
      printf("Pwm enable: %d\n", 2);
      fprintf(pwm, "2\n");
    }
    fflush(pwm);
    sleep(1);
  }
}

static void int_handler(__attribute__((unused)) int _) {
  printf("Exiting...\n");
  if (temp != NULL) {
    fclose(temp);
    temp = NULL;
  }
  if (pwm != NULL) {
    fprintf(pwm, "2\n");
    fflush(pwm);
    fclose(pwm);
    pwm = NULL;
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, int_handler);
  struct argument args = {
      "/sys/class/thermal/thermal_zone1/temp",
      "/sys/devices/platform/hp-wmi/hwmon/hwmon5/pwm1_enable",
      50000,
  };
  parse_args(argc, argv, &args);
  main_loop(args.temp_file, args.pwm_file, args.temp_wall);
  return 0;
}
