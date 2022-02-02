cmake_minimum_required(VERSION 2.8.9)
project(Hormann_door_pi)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)


include_directories(include)

set(libs 
/usr/lib/arm-linux-gnueabihf/libmosquittopp.so
/usr/lib/arm-linux-gnueabihf/libpthread.so
/usr/lib/arm-linux-gnueabihf/libyaml-cpp.so
)

set(INCLUDE_HEADER_FILES "include/*.h" )
file(GLOB mSOURCES  "src/*.cpp" )


add_executable(Hormann_door_pi "src/main.cpp" ${mSOURCES})
target_link_libraries(Hormann_door_pi  ${libs})

