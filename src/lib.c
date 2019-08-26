#include "lib.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int parse_port_number(const char* str) {
  char *end;
  const int64_t l = strtol(str, &end, 10);
  if (end == str || '\0' != *end || l > 0xffff || l < 1) {
    log_error(
      "port needs to be a number between 1 and 65535, given: '%s'",
      str
    );
    return -1;
  }
  return (int)l;
}

bool str_isnumber(const char* str) {
  const char* end = str + strlen(str);
  for (; str < end; ++str) {
    if (!isdigit(*str)) {
      return false;
    }
  }
  return true;
}

void split_filename_and_number(char* str, char** number) {
  char* nr = strchr(str, ':');
  if (nr) {
    *nr = '\0';
    ++nr;
    if (*nr) {
      *number = nr;
    }
  }
}

void _log(const char* level, const char* fmt, ...) {
  char now[] = "2019-08-12T18:44:30+0100";
  time_t unixtime;
  time(&unixtime);
  strftime(
    now,
    sizeof(now)/sizeof(*now),
    "%Y-%m-%dT%H:%M:%S%z",
    localtime(&unixtime)
  );
  printf("%s [%s] - ", now, level);
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  puts("");
}
