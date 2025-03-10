# Create an executable to test RDRAND
add_executable(rdrand_test rdrand.c)

# Define installation rules
install(TARGETS rdrand_test RUNTIME DESTINATION bin)
install(FILES rdrand.h DESTINATION include)
