find_program(
  IWYU_BIN
  NAMES iwyu_tool iwyu_tool.py
)

add_custom_target(
  iwyu
  "${IWYU_BIN}" -p .
)
