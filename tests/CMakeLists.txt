
add_executable(lokineo_tests)
target_sources(lokineo_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(lokineo_tests PRIVATE lokineo::loki)
