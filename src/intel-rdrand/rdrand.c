#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include <cpuid.h>

#define DEFAULT_SIZE_GB 0.1
#define RETRY_LIMIT 10
#define BYTES_IN_GB (1024 * 1024 * 1024)
#define DEFAULT_OUTPUT_FILE "rdrand_output.bin"

// Define the number of random values to buffer before writing
#define BUFFER_SIZE (1024 * 1024 * 4)  // 4 * 1,048,576 uint64_t numbers (~32 MB)

#define PROGRESS_UPDATE_INTERVAL 1024 * 1024  // Update progress every 1MB

int is_rdrand_supported() {
    uint32_t eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        return (ecx & (1 << 30)) != 0;  // Check if RDRAND is supported (bit 30 of ECX)
    }
    return 0;
}

int get_rdrand_uint64(uint64_t *rand_val) {
    unsigned long long temp_val;
    for (int i = 0; i < RETRY_LIMIT; i++) {
        if (_rdrand64_step(&temp_val)) {
            *rand_val = (uint64_t)temp_val;
            return 1;
        }
    }
    return 0;  // Failed after multiple attempts
}

int main(int argc, char *argv[]) {
    if (!is_rdrand_supported()) {
        printf("RDRAND is not supported on this CPU.\n");
        return 1;
    }

    double data_size_gb = DEFAULT_SIZE_GB;
    const char *output_path = DEFAULT_OUTPUT_FILE;

    if (argc > 1) {
        double size_arg = atof(argv[1]);
        if (size_arg > 0) {
            data_size_gb = size_arg;
        } else {
            printf("Invalid data size. Please enter a positive number (GB).\n");
            return 1;
        }
    }

    if (argc > 2) {
        output_path = argv[2];  // Use user-provided output file path
    }

    size_t data_size_bytes = (size_t)(data_size_gb * BYTES_IN_GB);
    size_t num_values = data_size_bytes / sizeof(uint64_t);

    printf("Generating %.2f GB (%zu bytes) of random data using RDRAND...\n", 
           data_size_gb, data_size_bytes);
    printf("Writing output to: %s\n", output_path);

    FILE *output_file = fopen(output_path, "wb");
    if (!output_file) {
        perror("Failed to open output file");
        return 1;
    }

    // Allocate a buffer to hold random numbers before writing to file
    uint64_t *buffer = malloc(BUFFER_SIZE * sizeof(uint64_t));
    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        fclose(output_file);
        return 1;
    }
    
    uint64_t random_value;
    size_t buf_index = 0;
    
    size_t values_generated = 0;
    size_t last_progress_update = 0;
    
    for (size_t i = 0; i < num_values; i++) {
        if (!get_rdrand_uint64(&random_value)) {
            printf("\nRDRAND failed to generate a valid random number.\n");
            free(buffer);
            fclose(output_file);
            return 1;
        }
        buffer[buf_index++] = random_value;
        values_generated++;
        
        // Show progress
        if (values_generated - last_progress_update >= PROGRESS_UPDATE_INTERVAL / sizeof(uint64_t)) {
            double progress = (double)values_generated / num_values * 100;
            printf("\rProgress: %.1f%% (%.2f GB / %.2f GB)", 
                   progress,
                   (double)values_generated * sizeof(uint64_t) / BYTES_IN_GB,
                   data_size_gb);
            fflush(stdout);
            last_progress_update = values_generated;
        }
        
        // If the buffer is full, write its contents to the file
        if (buf_index == BUFFER_SIZE) {
            if (fwrite(buffer, sizeof(uint64_t), BUFFER_SIZE, output_file) != BUFFER_SIZE) {
                printf("\nFailed to write data to file\n");
                free(buffer);
                fclose(output_file);
                return 1;
            }
            buf_index = 0;
        }
    }
    
    // Write any remaining values in the buffer
    if (buf_index > 0) {
        if (fwrite(buffer, sizeof(uint64_t), buf_index, output_file) != buf_index) {
            perror("Failed to write remaining data to file");
            free(buffer);
            fclose(output_file);
            return 1;
        }
    }
    
    free(buffer);
    fclose(output_file);
    printf("\nRandom data successfully written to '%s'.\n", output_path);
    
    return 0;
}
