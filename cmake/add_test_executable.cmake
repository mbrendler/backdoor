
function(add_test_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} cmocka)
  add_test(${target} ${target})
endfunction()

function(target_wrap_function target)
  foreach(fn IN LISTS ARGN)
    target_link_options("${target}" PUBLIC "-Wl,--wrap,${fn}")
  endforeach()
endfunction()
