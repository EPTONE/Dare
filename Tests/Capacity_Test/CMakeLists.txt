project(cap_test LANGUAGES C VERSION 0.0.0)
add_executable(${PROJECT_NAME} capacity_test.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${P_NAME})
