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
        test.c Config.c Config.h CryptoFunctions.h CryptoFunctions.c)

target_link_libraries(${PROJECT_NAME} -lcrypto)
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.o -ldl -lpthread)
