/*
 * VBSLite Subscriber (Halo OS)
 * Receives sensor data and toggles GPIO for E2E measurement
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <vbslite/Rte_Dds.h>
#include <vcos/vcos_gpio.h>

#define TOPIC_NAME "SensorData"

typedef struct {
    uint64_t timestamp_us;
    float imu_accel_x;
    float imu_accel_y;
    float imu_accel_z;
    uint32_t sequence;
} SensorData_t;

static uint64_t latencies[10000];
static int latency_count = 0;

static inline uint64_t get_timestamp_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

void data_callback(void *data, size_t size) {
    SensorData_t *sensor = (SensorData_t *)data;
    uint64_t now = get_timestamp_us();
    
    /* Calculate E2E latency */
    uint64_t latency = now - sensor->timestamp_us;
    
    if (latency_count < 10000) {
        latencies[latency_count++] = latency;
    }
    
    if (sensor->sequence % 1000 == 0) {
        printf("Received seq %u, E2E latency: %lu µs\n", 
               sensor->sequence, latency);
    }
}

int main(void) {
    printf("=== Halo OS VBSLite Subscriber ===\n");
    
    /* Initialize */
    Rte_Dds_Init();
    
    /* Create subscriber */
    Rte_Dds_Subscriber_t *sub;
    Rte_Dds_CreateSubscriber(TOPIC_NAME, sizeof(SensorData_t), 
                             data_callback, &sub);
    
    /* Configure GPIO output (for actuator simulation) */
    vcos_gpio_t gpio_actuator;
    vcos_gpio_init(&gpio_actuator, GPIO_PIN_21);  // P20.1
    vcos_gpio_set_direction(&gpio_actuator, VCOS_GPIO_OUTPUT);
    
    printf("Listening on topic '%s'\n", TOPIC_NAME);
    printf("Press Ctrl+C to stop\n\n");
    
    /* Wait for data (callbacks handle reception) */
    sleep(60);  // Run for 60 seconds
    
    /* Calculate stats */
    uint64_t min = UINT64_MAX, max = 0, sum = 0;
    for (int i = 0; i < latency_count; i++) {
        if (latencies[i] < min) min = latencies[i];
        if (latencies[i] > max) max = latencies[i];
        sum += latencies[i];
    }
    
    printf("\nE2E Latency Statistics:\n");
    printf("  Min:    %lu µs\n", min);
    printf("  Avg:    %lu µs\n", sum / latency_count);
    printf("  Max:    %lu µs\n", max);
    
    /* Verdict on <1ms claim */
    uint64_t avg = sum / latency_count;
    if (avg < 1000) {
        printf("✓ PASS: Validates <1ms claim (avg = %lu µs)\n", avg);
    } else {
        printf("✗ FAIL: Does not meet <1ms (avg = %lu µs)\n", avg);
    }
    
    /* Cleanup */
    Rte_Dds_DeleteSubscriber(sub);
    Rte_Dds_Deinit();
    
    return 0;
}
