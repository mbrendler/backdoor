cmake_minimum_required(VERSION 3.9.0)
project("backdoor" C)

include(cmake/add_test_executable.cmake)
include(cmake/clang-tidy.cmake)
include(cmake/cppcheck.cmake)
include(cmake/iwyu.cmake)

if(APPLE)
  include_directories(SYSTEM /usr/local/include)
  link_directories(/usr/local/lib)
endif()

if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
  -Wall \
  -Wextra \
  -Wpedantic \
  -Wno-missing-prototypes \
  -Wno-padded \
  -Wno-gnu-zero-variadic-macro-arguments \
  -Wno-unknown-pragmas \
")

set(IPO_SUPPORT FALSE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT IPO_SUPPORT OUTPUT output)
endif()

add_executable(backdoor
  src/backdoor.c
  src/server.c
  src/commands.c
  src/lib.c
)
set_property(
  TARGET backdoor
  PROPERTY INTERPROCEDURAL_OPTIMIZATION ${IPO_SUPPORT}
)

enable_testing()

add_test_executable(test_lib test/test_lib.c src/lib.c)
add_test_executable(test_commands test/test_commands.c src/commands.c src/lib.c)

if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
  add_test_executable(test_backdoor
    test/test_backdoor.c
    src/backdoor.c
    src/lib.c
  )
  target_wrap_function(test_backdoor exit printf)

  add_test_executable(test_server
    test/test_server.c
    src/server.c
  )
  target_wrap_function(test_server
    socket
    bind
    listen
    accept
    read
    system
    close
  )
endif()
