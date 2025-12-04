```c
/*
 * Halo OS Crypto Performance Benchmark
 * Tests AES-256-GCM using hardware crypto engine
 */

#include 
#include 
#include 
#include 
#include <crypto/crypto_hw.h>

#define BLOCK_SIZE (1024 * 1024)  // 1 MB
#define ITERATIONS 1000

int main(void) {
    printf("=== Halo OS Crypto Benchmark ===\n");
    
    /* Allocate buffers */
    uint8_t *plaintext = malloc(BLOCK_SIZE);
    uint8_t *ciphertext = malloc(BLOCK_SIZE);
    uint8_t key[32] = {0};  // 256-bit key
    uint8_t iv[16] = {0};
    
    /* Fill with test data */
    memset(plaintext, 0xAA, BLOCK_SIZE);
    
    /* Initialize hardware crypto */
    crypto_hw_init();
    crypto_hw_aes_init(CRYPTO_AES_256_GCM, key, 32);
    
    /* Benchmark */
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < ITERATIONS; i++) {
        crypto_hw_aes_encrypt(plaintext, ciphertext, BLOCK_SIZE, iv);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    /* Calculate throughput */
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    double total_mb = (BLOCK_SIZE * ITERATIONS) / (1024.0 * 1024.0);
    double throughput_mbs = total_mb / elapsed;
    double throughput_gbs = throughput_mbs / 1024.0;
    
    printf("Results:\n");
    printf("  Block size:  %d bytes\n", BLOCK_SIZE);
    printf("  Iterations:  %d\n", ITERATIONS);
    printf("  Total data:  %.2f MB\n", total_mb);
    printf("  Time:        %.2f seconds\n", elapsed);
    printf("  Throughput:  %.2f GB/s\n", throughput_gbs);
    
    free(plaintext);
    free(ciphertext);
    
    return 0;
}
```
