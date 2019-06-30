#ifndef daman_imu_h
#define daman_imu_h
#include "global_types.h"

void ImuHandle (packet_t *packet);
void ImuInit (void);
void ImuConfig (imuAccelConfig_t imuAccelConfig, imuGyroConfig_t imuGyroConfig);

#endif