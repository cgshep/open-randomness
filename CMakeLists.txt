cmake_minimum_required(VERSION 3.10)
project(OpenRandomness C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mrdrnd")

# Add include directory to include path
include_directories(${PROJECT_SOURCE_DIR}/include)

# Ensure data and results directories exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/results)

# Add subdirectories for each component
add_subdirectory(src/intel-rdrand)

# Install scripts manually (since they are shell scripts)
install(PROGRAMS src/linux_urandom/urandom_sample.sh DESTINATION bin)
install(PROGRAMS src/intel-rdrand/rdrand_sample.sh DESTINATION bin)

# Add custom target for 'make run'
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E echo "Running rdrand_test..."
    COMMAND ${CMAKE_BINARY_DIR}/src/intel-rdrand/rdrand_test 0.1 ${CMAKE_SOURCE_DIR}/data/rdrand_output.bin
    COMMAND ${CMAKE_COMMAND} -E echo "Running urandom_sample.sh..."
    COMMAND ${CMAKE_SOURCE_DIR}/src/linux_urandom/urandom_sample.sh ${CMAKE_SOURCE_DIR}/data/urandom_${OS_VERSION}_${DATE}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating random data..."
)

# Make analyze.sh executable
execute_process(COMMAND chmod +x ${CMAKE_SOURCE_DIR}/analyze.sh)

# Add analyze targets
add_custom_target(analyze-intel-rdrand
    COMMAND ${CMAKE_SOURCE_DIR}/analyze.sh ${CMAKE_SOURCE_DIR}/data/rdrand_output.bin
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Analyzing Intel RDRAND output..."
    DEPENDS run
)

add_custom_target(analyze-linux-urandom
    COMMAND ${CMAKE_SOURCE_DIR}/analyze.sh ${CMAKE_SOURCE_DIR}/data/urandom_output.bin
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Analyzing Linux urandom output..."
    DEPENDS run
)

# Meta-target to analyze all
add_custom_target(analyze
    COMMAND ${CMAKE_MAKE_PROGRAM} analyze-intel-rdrand
    COMMAND ${CMAKE_MAKE_PROGRAM} analyze-linux-urandom
    COMMENT "Analyzing all random number generators..."
)

# Add analyze-all-data target
add_custom_target(analyze-all-data
    COMMAND find ${CMAKE_SOURCE_DIR}/data -name "*.bin" -exec ${CMAKE_SOURCE_DIR}/analyze.sh {} \;
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