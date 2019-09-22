
function(add_test_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} cmocka)
  set_target_properties(${target} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE
    COMPILE_DEFINITIONS TEST
    # -fno-builtin is needed to mock exit calls
    COMPILE_FLAGS "-g -O0 -fno-builtin"
  )
  add_test(${target} ${target})
endfunction()

function(target_wrap_function target)
  foreach(fn IN LISTS ARGN)
    target_link_options("${target}" PUBLIC "-Wl,--wrap,${fn}")
  endforeach()
endfunction()
