/*
 * AUTOSAR SOME/IP Publisher (Vector stack)
 * Publishes sensor data over Ethernet using SOME/IP protocol
 */

#include <stdio.h>
#include <stdint.h>
#include "Rte_SensorPublisher.h"
#include "SomeIp_Sd.h"

#define SERVICE_ID 0x1234
#define EVENT_ID 0x8001

typedef struct {
    uint64 timestamp_us;
    float32 imu_accel_x;
    float32 imu_accel_y;
    float32 imu_accel_z;
    uint32 sequence;
} SensorData;

Std_ReturnType Runnable_Publish(void) {
    static uint32 seq = 0;
    SensorData data;
    
    /* Get timestamp */
    data.timestamp_us = GetCurrentTimeMicros();
    data.imu_accel_x = 0.1f * seq;
    data.imu_accel_y = 0.2f * seq;
    data.imu_accel_z = 9.8f;
    data.sequence = seq++;
    
    /* Serialize and send via SOME/IP */
    Rte_ISignal_SensorEvent_Send(&data, sizeof(data));
    
    return E_OK;
}

/* Main function (AUTOSAR Adaptive style) */
int main(void) {
    printf("=== AUTOSAR SOME/IP Publisher ===\n");
    
    /* Initialize SOME/IP stack */
    SomeIpSd_Init(NULL);
    
    /* Offer service */
    SomeIpSd_OfferService(SERVICE_ID, 1, 0);
    
    /* Cyclically publish */
    while (1) {
        Runnable_Publish();
        usleep(1000);  // 1kHz
    }
    
    return 0;
}
