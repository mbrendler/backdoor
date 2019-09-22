#pragma once

#include <stdnoreturn.h>

#ifndef TEST
#  define NORETURN noreturn
#else
#  define NORETURN
#endif

NORETURN void server_run(const char* address, int port);
