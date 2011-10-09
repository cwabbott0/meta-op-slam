#include "imu3000.h"
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ratelimit.h>
#include <linux/kfifo.h>

static int gpio_pin = GPIO_PIN;
int imu3000_accel_is_little_endian;
static unsigned int irq;

module_param(gpio_pin, int, S_IRUGO);

static struct work_struct imu3000_work;

struct imu3000_queue imu3000_queue = {
	.wp = 0,
	.rp = 0,
};

DECLARE_WAIT_QUEUE_HEAD(imu3000_wait_queue);

static void read_data(void)
{
	u16 fifo_count;
	u8 fifo_data[512], fifo_count_arr[2];
	int i, j;

	imu3000_i2c_read(imu3000_i2c_client, 0x3a, fifo_count_arr, 2);
	fifo_count = fifo_count_arr[0] << 8 | fifo_count_arr[1];

	if((fifo_count % 16 != 0 && fifo_count % 16 != 2) || fifo_count < 16) {
		/*printk(KERN_WARNING "imu3000: fifo_count is not expected, got %d\n", fifo_count);*/
		return;
	}

	imu3000_i2c_read(imu3000_i2c_client, 0x3c, fifo_data, fifo_count - 2);

	if(imu3000_num_readers == 0) /*only copy data if someone's listening */
		return;

	for(i = fifo_count % 16 ? 2 : 0; i < fifo_count; i += 16) {
		struct imu3000_data data;
		data.temp = fifo_data[i] << 8 | fifo_data[i + 1];
		for(j = 0; j < 3; j++) {
			data.gyro_data[j] = fifo_data[i + 2 + 2 * j] << 8 | fifo_data[i + 3 + 2 * j];
			if(imu3000_accel_is_little_endian)
				data.accel_data[j] = fifo_data[i + 9 + 2 * j] << 8 | fifo_data[i + 8 + 2 * j];
			else
				data.accel_data[j] = fifo_data[i + 8 + 2 * j] << 8 | fifo_data[i + 9 + 2 * j];
		}
		/*printk("%-8d%-8d%-8d%-8d%-8d%-8d%-8d\n", data.temp,
			data.gyro_data[0], data.gyro_data[1], data.gyro_data[2],
			data.accel_data[0], data.accel_data[1], data.accel_data[2]);*/
		/*printk("wp: %d, rp %d\n", imu3000_queue.wp, imu3000_queue.rp);*/
		if((imu3000_queue.wp + 1) % FIFO_SIZE != imu3000_queue.rp) {
			imu3000_queue.data[imu3000_queue.wp] = data;
			imu3000_queue.wp = (imu3000_queue.wp + 1) % FIFO_SIZE;
			wake_up(&imu3000_wait_queue);
		}
	}
}

static irqreturn_t irq_handler(int irq, void *dev_id)
{
	schedule_work(&imu3000_work);
	return IRQ_HANDLED;
}

static void work_handler(struct work_struct *work)
{
	u8 value;

	if(down_killable(&imu3000_client_lock))
		return;
	if(!imu3000_i2c_client) {
		up(&imu3000_client_lock);
		return; /*it's been destroyed already by imu3000_i2c_cleanup(), or a stray interrupt before i2c is initialized */
	}
	imu3000_i2c_read(imu3000_i2c_client, 0x1a, &value, 1); /*resets the interrupt */

	if(value & 0x80)
		imu3000_i2c_write_byte(imu3000_i2c_client, 0x3d, 0x02); /* reset FIFO */
	else if(value & 0x01) 
		read_data();
	if(value & 0x08)
		printk(KERN_WARNING "imu3000: accel NAK\n");

	up(&imu3000_client_lock);
}

int __init imu3000_init_irq(void)
{
	int ret;

	INIT_WORK(&imu3000_work, work_handler);

	if(!gpio_is_valid(gpio_pin)) {
		printk(KERN_WARNING "imu3000: invalid gpio\n");
		return -EINVAL;
	}

	ret = gpio_request_one(gpio_pin, GPIOF_IN, "imu3000");
	if(ret) {
		printk(KERN_WARNING "imu3000: could not request gpio\n");
		return ret;
	}

	irq = gpio_to_irq(gpio_pin);
	if(irq < 0) {
		printk(KERN_WARNING "imu3000: could not get irq number\n");
		ret = -EINVAL;
		goto fail_1;
	}

	ret = request_irq(irq, irq_handler, IRQF_TRIGGER_RISING, "imu3000", NULL);
	if(ret < 0) {
		printk(KERN_WARNING "imu3000: could not install irq handler\n");
		goto fail_1;
	}

	return 0;

fail_1:
	gpio_free(gpio_pin);
	return ret;
}

void imu3000_cleanup_irq(void)
{
	free_irq(irq, NULL);
	gpio_free(gpio_pin);
}
