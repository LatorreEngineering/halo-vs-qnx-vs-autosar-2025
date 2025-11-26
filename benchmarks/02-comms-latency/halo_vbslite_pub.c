/*
 * VBSLite Publisher (Halo OS)
 * Publishes sensor data at 1kHz and toggles GPIO for E2E measurement
 * API: Fast DDS (eProsima) wrapper via Rte_Dds_*
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <vbslite/Rte_Dds.h>
#include <vcos/vcos_gpio.h>

#define TOPIC_NAME "SensorData"
#define PUB_RATE_HZ 1000

typedef struct {
    uint64_t timestamp_us;
    float imu_accel_x;
    float imu_accel_y;
    float imu_accel_z;
    uint32_t sequence;
} SensorData_t;

int main(void) {
    printf("=== Halo OS VBSLite Publisher ===\n");
    
    /* Initialize VBSLite */
    if (Rte_Dds_Init() != RTE_E_OK) {
        fprintf(stderr, "Failed to init VBSLite\n");
        return 1;
    }
    
    /* Create publisher */
    Rte_Dds_Publisher_t *pub;
    if (Rte_Dds_CreatePublisher(TOPIC_NAME, sizeof(SensorData_t), &pub) != RTE_E_OK) {
        fprintf(stderr, "Failed to create publisher\n");
        return 1;
    }
    
    /* Configure GPIO for E2E measurement (toggle on publish) */
    vcos_gpio_t gpio_trigger;
    vcos_gpio_init(&gpio_trigger, GPIO_PIN_20);  // P20.0 on TC397
    vcos_gpio_set_direction(&gpio_trigger, VCOS_GPIO_OUTPUT);
    
    /* Publishing loop */
    SensorData_t data;
    uint32_t seq = 0;
    
    printf("Publishing at %d Hz on topic '%s'\n", PUB_RATE_HZ, TOPIC_NAME);
    printf("Press Ctrl+C to stop\n\n");
    
    while (1) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        data.timestamp_us = ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
        
        /* Simulate IMU data */
        data.imu_accel_x = 0.1f * seq;
        data.imu_accel_y = 0.2f * seq;
        data.imu_accel_z = 9.8f;
        data.sequence = seq++;
        
        /* Toggle GPIO HIGH before publish */
        vcos_gpio_write(&gpio_trigger, 1);
        
        /* Publish */
        Rte_Dds_Publish(pub, &data);
        
        /* Toggle GPIO LOW after publish */
        vcos_gpio_write(&gpio_trigger, 0);
        
        if (seq % 1000 == 0) {
            printf("Published %u samples\n", seq);
        }
        
        usleep(1000000 / PUB_RATE_HZ);  // 1ms
    }
    
    Rte_Dds_DeletePublisher(pub);
    Rte_Dds_Deinit();
    
    return 0;
}
