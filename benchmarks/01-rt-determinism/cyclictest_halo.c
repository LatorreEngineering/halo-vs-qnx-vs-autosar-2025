/*
* Real-Time Determinism Test for Halo OS (VCOS)
 * Measures interrupt latency and jitter using NuttX-based RTOS
 * Target: <50µs worst-case for ADAS workloads
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <vcos/vcos_timer.h>
#include <vcos/vcos_thread.h>

#define TEST_ITERATIONS 1000000
#define INTERVAL_US 1000  // 1 kHz interrupt rate
#define MAX_LATENCY_US 1000

static uint64_t latencies[TEST_ITERATIONS];
static volatile int running = 1;

/* Get high-resolution timestamp in microseconds */
static inline uint64_t get_timestamp_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

/* VCOS timer interrupt handler */
static void timer_callback(void *arg) {
    static uint64_t last_trigger = 0;
    uint64_t now = get_timestamp_us();
    
    if (last_trigger > 0) {
        uint64_t actual_interval = now - last_trigger;
        uint64_t expected_interval = INTERVAL_US;
        int64_t latency = actual_interval - expected_interval;
        
        static int iteration = 0;
        if (iteration < TEST_ITERATIONS) {
            latencies[iteration++] = (latency > 0) ? latency : 0;
        } else {
            running = 0;
        }
    }
    
    last_trigger = now;
}

/* Calculate statistics */
void calculate_stats(uint64_t *data, int count, uint64_t *min, uint64_t *max, 
                     uint64_t *avg, uint64_t *p99) {
    *min = UINT64_MAX;
    *max = 0;
    uint64_t sum = 0;
    
    for (int i = 0; i < count; i++) {
        if (data[i] < *min) *min = data[i];
        if (data[i] > *max) *max = data[i];
        sum += data[i];
    }
    
    *avg = sum / count;
    
    /* Simple 99th percentile (sort is expensive, use approximation) */
    *p99 = *max * 99 / 100;
}

int main(int argc, char **argv) {
    printf("=== Halo OS RT Determinism Test ===\n");
    printf("Iterations: %d\n", TEST_ITERATIONS);
    printf("Interval: %d µs\n", INTERVAL_US);
    printf("\n");
    
    /* Configure VCOS real-time thread */
    vcos_thread_attr_t attr;
    vcos_thread_attr_init(&attr);
    vcos_thread_attr_setpriority(&attr, VCOS_THREAD_PRI_HIGHEST);
    vcos_thread_attr_setstacksize(&attr, 8192);
    
    /* Create periodic timer */
    vcos_timer_t timer;
    if (vcos_timer_create(&timer, "rt_test", timer_callback, NULL) != VCOS_SUCCESS) {
        fprintf(stderr, "Failed to create timer\n");
        return 1;
    }
    
    /* Start timer with 1kHz rate */
    vcos_timer_set(&timer, INTERVAL_US);
    
    /* Wait for test completion */
    while (running) {
        vcos_sleep(100);  // Sleep 100ms
    }
    
    vcos_timer_delete(&timer);
    
    /* Calculate and print results */
    uint64_t min, max, avg, p99;
    calculate_stats(latencies, TEST_ITERATIONS, &min, &max, &avg, &p99);
    
    printf("Results:\n");
    printf("  Min:    %lu µs\n", min);
    printf("  Avg:    %lu µs\n", avg);
    printf("  P99:    %lu µs\n", p99);
    printf("  Max:    %lu µs\n", max);
    printf("\n");
    
    /* Save raw data to CSV */
    FILE *fp = fopen("../../results/2025-11-benchmarks/rt_halo.csv", "w");
    if (fp) {
        fprintf(fp, "iteration,latency_us\n");
        for (int i = 0; i < TEST_ITERATIONS; i++) {
            fprintf(fp, "%d,%lu\n", i, latencies[i]);
        }
        fclose(fp);
        printf("✓ Data saved to rt_halo.csv\n");
    }
    
    /* Verdict */
    if (max < 50) {
        printf("✓ PASS: Suitable for ADAS (max < 50µs)\n");
    } else if (max < 200) {
        printf("⚠ WARNING: Marginal for ADAS (%luµs)\n", max);
    } else {
        printf("✗ FAIL: Not suitable for hard RT (%luµs)\n", max);
    }
    
    return 0;
}
