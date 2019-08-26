#include "lib.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int port;
  const char* address;
} Parameters;

void parse_args(int argc, const char** argv, Parameters* parameters) {
  for (int i = 1; i < argc ; ++i) {
    if (0 == strcmp("--help", argv[i]) || 0 == strcmp("-h", argv[i])) {
      printf("%s [-h|--help] [BIND_ADDRESS] [PORT]\n\n", argv[0]);
      printf("  BIND_ADDRESS  --  default: %s\n", parameters->address);
      printf("  PORT          --  default: %d\n", parameters->port);
      exit(0);
    }
  }
  if (argc >= 3) {
    parameters->address = argv[1];
    parameters->port = parse_port_number(argv[2]);
  } else if (argc == 2) {
    if (str_isnumber(argv[1])) {
      parameters->port = parse_port_number(argv[1]);
    } else {
      parameters->address = argv[1];
    }
  }
  if (parameters->port < 0) {
    exit(1);
  }
}

int main(int argc, const char** argv) {
  Parameters parameters = { .address = "127.0.0.1", .port = 44444 };
  parse_args(argc, argv, &parameters);
  server_run(parameters.address, parameters.port);
}
