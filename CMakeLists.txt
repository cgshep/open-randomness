cmake_minimum_required(VERSION 3.10)
project(OpenRandomness C)

set(CMAKE_C_STANDARD 11)

# Add analyze-all-data target
add_custom_target(analyze
    COMMAND find ${CMAKE_SOURCE_DIR}/data -name "*.bin" -exec ${CMAKE_SOURCE_DIR}/analyze.sh {} \\\;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Analyzing all .bin files in data directory..."
)

# Add custom clean targets
add_custom_target(clean-data
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning data directory..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/data
    COMMENT "Removing generated random data..."
)

add_custom_target(clean-results
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning results directory..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/results
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/results
    COMMENT "Removing analysis results..."
)

add_custom_target(clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean-data
    COMMAND ${CMAKE_MAKE_PROGRAM} clean-results
    COMMENT "Cleaning all build artifacts, data, and results..."
)

# Make analyze.sh executable
execute_process(COMMAND chmod +x ${CMAKE_SOURCE_DIR}/analyze.sh)