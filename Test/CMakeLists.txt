add_test(
  NAME UnitTests 
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh" "${CMAKE_BINARY_DIR}/unit_tests"
)


file(GLOB PARSE_TESTS "${CMAKE_SOURCE_DIR}/examples/parse/*.pb")
foreach(TEST ${PARSE_TESTS}) 
  get_filename_component(TEST_NAME ${TEST} NAME_WE)
  add_test(
    NAME "parse-${TEST_NAME}"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/parse.sh" "${CMAKE_BINARY_DIR}/pbc" "${TEST}"
  )
endforeach()
