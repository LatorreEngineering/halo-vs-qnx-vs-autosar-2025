```c
/*
 * AUTOSAR SOME/IP Subscriber
 */

#include 
#include 
#include "Rte_SensorSubscriber.h"

typedef struct {
    uint64 timestamp_us;
    float32 imu_accel_x;
    float32 imu_accel_y;
    float32 imu_accel_z;
    uint32 sequence;
} SensorData;

static uint64_t latencies[10000];
static int count = 0;

void SensorEvent_Callback(SensorData *data) {
    uint64_t now = GetCurrentTimeMicros();
    uint64_t latency = now - data->timestamp_us;
    
    if (count < 10000) {
        latencies[count++] = latency;
    }
    
    if (data->sequence % 1000 == 0) {
        printf("Seq %u, E2E: %lu µs\n", data->sequence, latency);
    }
}

int main(void) {
    printf("=== AUTOSAR SOME/IP Subscriber ===\n");
    
    SomeIpSd_Init(NULL);
    Rte_ISignal_SensorEvent_Subscribe(SensorEvent_Callback);
    
    /* Wait for data */
    while (1) {
        usleep(10000);
    }
    
    return 0;
}
```
