#include "commands.h"
#include "lib.h"
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

static bool v_command_args_valid(char* buffer) {
  if (*buffer == '\'' || *buffer == '"') {
    ++buffer;
  }
  char* number = NULL;
  split_filename_and_number(buffer, &number);
  if (0 != access(buffer, R_OK)) {
    log_perror("(access) '%s'", buffer);
    return false;
  }
  if (number) {
    *(number - 1) = ':';
    char* last_char = number + strlen(number) - 1;
    char end_quote = *last_char == '"' || *last_char == '\'' ? *last_char : 0;
    if (end_quote) {
      *last_char = '\0';
    }
    if (!str_isnumber(number)) {
      log_error("invalid line number - '%s'", number);
      if (end_quote) {
        *last_char = end_quote;
      }
      return false;
    }
    if (end_quote) {
      *last_char = end_quote;
    }
  }
  return true;
}

typedef struct {
  const char* name;
  bool(*valid_args)(char*);
} Command;

static const Command COMMANDS[] = {
  { .name = "v ", .valid_args = v_command_args_valid },
};

bool command_valid(char* buffer) {
  const Command* commands_end = COMMANDS + sizeof(COMMANDS) / sizeof(*COMMANDS);
  for (const Command* c = COMMANDS; c < commands_end; ++c) {
    const size_t cmd_len = strlen(c->name);
    if (0 == strncmp(c->name, buffer, cmd_len)) {
      return c->valid_args(buffer + cmd_len);
    }
  }
  log_error("unknown command - '%s'", buffer);
  return false;
}
