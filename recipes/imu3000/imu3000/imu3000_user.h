#include <linux/types.h>

struct imu3000_data {
	__u16 temp, gyro_data[3], accel_data[3];
};
