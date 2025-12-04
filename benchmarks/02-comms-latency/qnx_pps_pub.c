```c
/*
 * QNX PPS (Persistent Publish/Subscribe) Publisher
 */

#include 
#include 
#include 
#include 
#include <sys/pps.h>
#include 

#define PPS_PATH "/pps/sensors/imu"

typedef struct {
    uint64_t timestamp_us;
    float imu_accel_x;
    float imu_accel_y;
    float imu_accel_z;
    uint32_t sequence;
} SensorData;

int main(void) {
    printf("=== QNX PPS Publisher ===\n");
    
    /* Create PPS object */
    int fd = open(PPS_PATH, O_WRONLY | O_CREAT, 0666);
    if (fd < 0) {
        perror("Failed to create PPS object");
        return 1;
    }
    
    SensorData data;
    uint32_t seq = 0;
    
    while (1) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        data.timestamp_us = ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
        
        data.imu_accel_x = 0.1f * seq;
        data.imu_accel_y = 0.2f * seq;
        data.imu_accel_z = 9.8f;
        data.sequence = seq++;
        
        /* Write to PPS (format: attr::value) */
        char buf[256];
        snprintf(buf, sizeof(buf),
                 "timestamp::%lu\naccel_x::%f\nsequence::%u\n",
                 data.timestamp_us, data.imu_accel_x, data.sequence);
        
        write(fd, buf, strlen(buf));
        
        if (seq % 1000 == 0) {
            printf("Published %u samples\n", seq);
        }
        
        usleep(1000);  // 1kHz
    }
    
    close(fd);
    return 0;
}
```
