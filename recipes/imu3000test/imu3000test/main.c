#include <linux/imu3000_user.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
	int ret, i, fd;

	printf("temp    gyro x  gyro y  gyro z  accel x accel y accel z\n");

	fd = open("/dev/imu3000", O_RDONLY, 0);
	if(fd < 0) {
		printf("failed to open file, errno %d\n", errno);
		return -1;
	}

	for(i = 0; i < 10; i++) {
		struct imu3000_data data;
		ret = read(fd, &data, sizeof(struct imu3000_data));
		if(ret < 0) {
			printf("failed to read file, errno %d\n", errno);
		}
		else
			//printf("read %d bytes\n", ret);
		printf("%-8d%-8d%-8d%-8d%-8d%-8d%-8d\n", data.temp,
			data.gyro_data[0], data.gyro_data[1], data.gyro_data[2],
			data.accel_data[0], data.accel_data[1], data.accel_data[2]);
	}
	close(fd);
	return 0;
}
