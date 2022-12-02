cmake_minimum_required(VERSION 3.10)

project(Jchar)
set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_DIRECTORY ./lib)

# set(SRC_SERVER ./src/server.cpp)
# set(SRC_CLIENT ./src/client.cpp)
set(SRC_HV_SERVER ./src/jchat_server.cpp)
set(SRC_HV_CLIETN ./src/jchat_client.cpp)

include_directories(./cJSON ./libhv/build/include/hv)

add_compile_options(-Wall -O2 -std=c++11) 

link_directories(./libhv/build/lib ./cJSON/build)

# add_executable(server ${SRC_SERVER})
# add_executable(client ${SRC_CLIENT})
add_executable(jchat_server ${SRC_HV_SERVER})
add_executable(jchat_client ${SRC_HV_CLIETN})

# add_library(cJSON STATIC ./src/cJSON.cpp)

# target_link_libraries(server cJSON pthread)
# target_link_libraries(client cJSON pthread)
target_link_libraries(jchat_server cjson hv pthread )
target_link_libraries(jchat_client cjson hv pthread ncursesw )