#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/i2c.h>
#include <linux/wait.h>
#include "imu3000_user.h"

/* default values for the module parameters (set up for the BeagleBoard currently) */
#define GPIO_PIN 132 /* MMC2_DAT0 */
#define I2C_BUS 2

#define FIFO_SIZE 8

struct imu3000_queue {
	struct imu3000_data data[FIFO_SIZE];
	unsigned int rp, wp;
};

extern struct i2c_client *imu3000_i2c_client;
extern struct semaphore imu3000_client_lock;
extern unsigned int imu3000_num_readers;
extern wait_queue_head_t imu3000_wait_queue;
extern int imu3000_accel_is_little_endian;
extern struct imu3000_queue imu3000_queue;

int imu3000_init_i2c(void) __init;
void imu3000_cleanup_i2c(void);
int imu3000_init_irq(void) __init;
void imu3000_cleanup_irq(void);

int imu3000_i2c_read(struct i2c_client *client, u8 reg_addr, u8 *buf, int count);
int imu3000_i2c_write_byte(struct i2c_client *client, u8 reg_addr, u8 data);

int imu3000_get_fifo(struct imu3000_data *data);
unsigned int imu3000_fifo_size(void);
void imu3000_fifo_reset(void);
