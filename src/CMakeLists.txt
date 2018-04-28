# Dawn Engine
# Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)

message("========== EXTERNAL ==========")
add_subdirectory(external)

message("========== DAWN ENGINE ==========")
add_subdirectory(dawn)

message("========== RENDERER TEST ==========")
add_subdirectory(renderer-test)

message("========== SANDBOX ==========")
add_subdirectory(sandbox)

message("========== SHOOTER ==========")
add_subdirectory(shooter)

