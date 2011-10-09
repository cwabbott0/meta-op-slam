#include "imu3000.h"
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/version.h>

static int i2c_bus = I2C_BUS;

module_param(i2c_bus, int, S_IRUGO);

static int __init imu3000_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __exit imu3000_remove(struct i2c_client *client);

struct i2c_client *imu3000_i2c_client;
DEFINE_SEMAPHORE(imu3000_client_lock);

static struct i2c_board_info imu3000_board_info = {
	I2C_BOARD_INFO("imu3000", 0)
};

static struct i2c_board_info accel_board_info = {
	I2C_BOARD_INFO("dummy", 0)
};

/* list of all the supported accels */
static const unsigned short accel_addr_list[] = {0x0f, I2C_CLIENT_END};

static const unsigned short imu3000_addr_list[] = {0x68, 0x69, I2C_CLIENT_END};

static const struct i2c_device_id imu3000_id[] = {
	{
		.name = "imu3000",
		.driver_data = 0,
	},
	{},
};

MODULE_DEVICE_TABLE(i2c, imu3000_id);

static struct i2c_driver imu3000_i2c_driver = {
	.driver = {
		.name = "imu3000",
		.owner = THIS_MODULE,
	},
	.id_table = imu3000_id,
	.probe = imu3000_probe,
	.remove = imu3000_remove,
};

int imu3000_i2c_read(struct i2c_client *client, u8 reg_addr, u8 *buf, int count)
{
	/* write register address, then read data */
	struct i2c_msg msgs[2];
	int ret;

	msgs[0].addr = msgs[1].addr = client->addr;
	msgs[0].len = 1;
	msgs[1].len = count;
	msgs[0].flags = msgs[1].flags = 0;
	msgs[1].flags |= I2C_M_RD;
	msgs[0].buf = &reg_addr;
	msgs[1].buf = buf;

	ret = i2c_transfer(client->adapter, msgs, 2);

	barrier();

	return (ret == 2) ? 0 : ret;
}

int imu3000_i2c_write_byte(struct i2c_client *client, u8 reg_addr, u8 data)
{
	int ret;
	u8 buf[2];
	buf[0] = reg_addr;
	buf[1] = data;
	ret = i2c_master_send(client, buf, 2);
	return (ret == 2) ? 0 : ret;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)

static int __imu3000_probe(struct i2c_adapter *adapter, unsigned short addr)
{
	struct i2c_msg msgs[2];
	int ret;
	u8 value, reg_addr = 0;

	msgs[0].addr = msgs[1].addr = addr;
	msgs[0].len = msgs[1].len = 1;
	msgs[0].flags = msgs[1].flags = 0;
	msgs[1].flags |= I2C_M_RD;
	msgs[0].buf = &reg_addr;
	msgs[1].buf = &value;

	ret = i2c_transfer(adapter, msgs, 2);

	barrier();

	return ret == 2 && value == 0x69;
}

#endif

int __init imu3000_init_i2c(void)
{
	int ret;
	struct i2c_adapter *adapter;

	ret = i2c_add_driver(&imu3000_i2c_driver);
	if(ret) {
		printk(KERN_WARNING "imu3000: Error registering i2c driver\n");
		return ret;
	}

	adapter = i2c_get_adapter(i2c_bus);

	if(!adapter) {
		printk(KERN_WARNING "imu3000: i2c_get_adapter(%d) failed\n", I2C_BUS);
		goto fail2;
	}

	if(down_killable(&imu3000_client_lock)) {
		ret = -ERESTARTSYS;
		goto fail1;
	}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
	imu3000_i2c_client = i2c_new_probed_device(adapter, &imu3000_board_info, imu3000_addr_list, __imu3000_probe);
#else
	imu3000_i2c_client = i2c_new_probed_device(adapter, &imu3000_board_info, imu3000_addr_list);
#endif
	if(!imu3000_i2c_client) {
		printk(KERN_WARNING "imu3000: i2c_new_probed_device() failed\n");
		goto fail1;
	}

	up(&imu3000_client_lock);

	i2c_put_adapter(adapter);

	return 0;

	fail1:
	i2c_put_adapter(adapter);

	fail2:
	i2c_del_driver(&imu3000_i2c_driver);

	return -1;
}

void imu3000_cleanup_i2c(void)
{
	if(down_killable(&imu3000_client_lock))
		return;
	if(imu3000_i2c_client) {
		i2c_unregister_device(imu3000_i2c_client);
		imu3000_i2c_client = NULL;
	}

	i2c_del_driver(&imu3000_i2c_driver);
	up(&imu3000_client_lock);
}

static int kxtf9_config(struct i2c_client *imu3000_client, struct i2c_client *client)
{
	int ret;

	imu3000_accel_is_little_endian = 1;

	/* CTRL_REG1 */
	ret = imu3000_i2c_write_byte(client, 0x1b, 0x48);
	if(ret)
		return ret;

	/* CTRL_REG2 */
	ret = imu3000_i2c_write_byte(client, 0x1c, 0);
	if(ret)
		return ret;

	/* DATA_CTRL_REG (sets output to 200hz) */
	ret = imu3000_i2c_write_byte(client, 0x21, 0x04);
	if(ret)
		return ret;

	/* CTRL_REG1 - enables PC1 */
	ret = imu3000_i2c_write_byte(client, 0x1b, 0xc8);
	if(ret)
		return ret;

	/* tell the imu3000 where to get the data from */
	ret = imu3000_i2c_write_byte(imu3000_client, 0x14, 0x0f);
	if(ret)
		return ret;

	ret = imu3000_i2c_write_byte(imu3000_client, 0x18, 0x06);
	if(ret)
		return ret;

	return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)

/* try to read the WHO_AM_I register; the default value is 1 */ 
static int kxtf9_probe(struct i2c_adapter *adapter)
{
	struct i2c_msg msgs[2];
	u8 reg_addr = 0x0f, value;
	int ret;

	msgs[0].addr = msgs[1].addr = 0x0f;
	msgs[0].len = msgs[1].len = 1;
	msgs[0].flags = msgs[1].flags = 0;
	msgs[1].flags |= I2C_M_RD;
	msgs[0].buf = &reg_addr;
	msgs[1].buf = &value;

	ret = i2c_transfer(adapter, msgs, 2);

	return (ret == 2 && value == 1);
}

static int accel_probe(struct i2c_adapter *adapter, unsigned short addr)
{
	switch(addr) {
	case 0x0f:
		return kxtf9_probe(adapter);
	}
	return 0;
}

#endif

static int imu3000_configure_accel(struct i2c_client *imu3000_client)
{
	int ret;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
	struct i2c_client *accel_client = i2c_new_probed_device(imu3000_client->adapter,
						&accel_board_info, accel_addr_list, accel_probe);
#else
	struct i2c_client *accel_client = i2c_new_probed_device(imu3000_client->adapter,
						&accel_board_info, accel_addr_list);
#endif

	if(!accel_client) {
		printk(KERN_WARNING "imu3000: failed to find a suitable accelerometer.\n");
		return -1;
	}

	switch(accel_client->addr) {
	case 0x0f:
		printk(KERN_ALERT "imu3000: found kxtf9, configuring...\n");
		ret = kxtf9_config(imu3000_client, accel_client);
		break;
	default:
		printk(KERN_WARNING "imu3000: unknown address %d", (int) accel_client->addr);
		ret = -1;
	}

	if(ret)
		printk(KERN_WARNING "imu3000: failed to configure accelerometer.\n");

	i2c_unregister_device(accel_client);

	return ret;
}

static int imu3000_configure(struct i2c_client *client)
{
	int ret;

	/* make sure we're in passthrough mode so the accel shows up */
	ret = imu3000_i2c_write_byte(client, 0x3d, 0);
	if(ret)
		return ret;

	/* configure the DPLF and scale selection (500 deg/sec, 256 Hz, 8 kHz sample rate) */
	ret = imu3000_i2c_write_byte(client, 0x16, 0x08);
	if(ret)
		return ret;

	/* configure sample rate divider for 160 Hz */
	ret = imu3000_i2c_write_byte(client, 0x15, 49);
	if(ret)
		return ret;

	ret = imu3000_configure_accel(client);
	if(ret)
		return ret;

	/* configure the FIFO queue */
	ret = imu3000_i2c_write_byte(client, 0x12, 0xff);
	if(ret)
		return ret;

	/* configure the interrupt */
	ret = imu3000_i2c_write_byte(client, 0x17, 0x29);
	if(ret)
		return ret;

	/* finally, enable and reset everything */
	ret = imu3000_i2c_write_byte(client, 0x3d, 0x6b);
	if(ret)
		return ret;

	return 0;
}

static int __init imu3000_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;

	printk(KERN_ALERT "imu3000: configuring...\n");
	ret = imu3000_configure(client);
	if(ret)
		printk(KERN_WARNING "imu3000: failed to configure\n");

	return ret;
}

static int __exit imu3000_remove(struct i2c_client *client)
{
	return 0;
}	
