file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(threads_lib ${HEADERS} ${SOURCES})
target_include_directories(threads_lib PUBLIC include)
