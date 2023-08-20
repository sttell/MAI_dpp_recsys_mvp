cmake_minimum_required(VERSION 3.20)

project(
    recsys_t2s_test
    VERSION 1.0.0
    DESCRIPTION "Target for testing Team2Students recommendation system service."
)

enable_testing()

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
        main.cpp
        common/education_level_test.cpp

        students_post_test.cpp
        )


target_link_libraries(${PROJECT_NAME} GTest::GTest pthread)
target_include_directories(${PROJECT_NAME} PRIVATE ${GTest_INCLUDE_PATH})

