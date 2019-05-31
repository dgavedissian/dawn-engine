# Dawn Engine
# Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)

add_executable(pkg Main.cpp)
enable_cpp11(pkg)
target_link_libraries(pkg args optional-lite json)