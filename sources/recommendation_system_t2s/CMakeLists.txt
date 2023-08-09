cmake_minimum_required(VERSION 3.2)

set(PROJECT_NAME recommendations_system_t2s)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(PROJECT_MESSAGE_PREFIX "Recommendations System T2S")

project(${PROJECT_NAME} C CXX)

set(STD_CXX "c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    project_message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
include_directories(service)
include_directories(service/handlers/include)

link_directories("/usr/local/lib")

add_executable(${EXECUTABLE_NAME}
        main.cpp
        service/http_server/http_server.cpp

        service/handlers/sources/request_handler_factory.cpp
        service/handlers/sources/base_handler/base_request_handler.cpp
        service/handlers/sources/handler_factory.cpp
        service/handlers/sources/team_handler/team_request_handler.cpp
        service/handlers/sources/html_form/html_form.cpp
        service/handlers/sources/student_handler/student_request_handler.cpp

        service/database/database.cpp
        service/database/student.cpp
        service/database/student_indexer.cpp
        service/database/student_indexer.hpp

        service/config/server_config.cpp
        service/config/path_validate.cpp

        service/common/errors.cpp

        )

target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/service")

target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)


target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB
)
