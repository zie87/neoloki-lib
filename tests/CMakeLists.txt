set(TEST_APP_NAME "neoloki_tests")

add_executable(${TEST_APP_NAME})
target_sources(${TEST_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${TEST_APP_NAME} PRIVATE lokineo::loki)

add_test(build_test_app "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target ${TEST_APP_NAME})
add_test(neoloki_unittests ${TEST_APP_NAME})
set_tests_properties(neoloki_unittests PROPERTIES DEPENDS build_test_app)
