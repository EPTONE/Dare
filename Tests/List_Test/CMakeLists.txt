project(list_test LANGUAGES C VERSION 0.0.0)
add_executable(${PROJECT_NAME} list_test.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${P_NAME})
add_test(NAME ${PROJECT_NAME} COMMAND ./${PROJECT_NAME})
