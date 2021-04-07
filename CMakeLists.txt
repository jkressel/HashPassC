cmake_minimum_required(VERSION 3.17)
project(HashPassC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(.)


add_executable(HashPassC
        DBFunctions.h
        DBFunctions.c
        PasswordFunctions.c
        PasswordFunctions.h
        Phrase.c
        Phrase.h
        Salt.c
        Salt.h
        hashpass.c Config.c Config.h CryptoFunctions.h CryptoFunctions.c)

target_link_libraries(${PROJECT_NAME} -lcrypto)
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.o -ldl -lpthread)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

target_link_libraries(HashPassC ${GTK3_LIBRARIES})
