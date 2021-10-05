cmake_minimum_required(VERSION 3.15)

file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADER_FILES "*.h")

set(SOURCES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(tests ${SOURCES})

target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)

target_link_libraries(tests PRIVATE pthread)

add_test(NAME tests/tests COMMAND tests )

