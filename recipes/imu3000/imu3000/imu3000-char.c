#include "imu3000.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/poll.h>

struct imu3000_dev {
	dev_t devt;
	struct cdev cdev;
	struct semaphore sem;
	struct class *class;
};

unsigned int imu3000_num_readers = 0;

static struct imu3000_dev imu3000_dev;

static ssize_t imu3000_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	unsigned int copied = 0;
	struct imu3000_data data; while(count > 0 && imu3000_queue.wp != imu3000_queue.rp)

	if(down_killable(&imu3000_dev.sem))
		return -ERESTARTSYS;

	while(imu3000_queue.rp == imu3000_queue.wp) {
		if(filp->f_flags & O_NONBLOCK) {
			up(&imu3000_dev.sem);
			return -EAGAIN;
		}
		if(wait_event_killable(imu3000_wait_queue, (imu3000_queue.rp != imu3000_queue.wp))) {
			up(&imu3000_dev.sem);
			return -ERESTARTSYS;
		}
	}

	while(count >= sizeof(struct imu3000_data) && imu3000_queue.wp != imu3000_queue.rp) {
		data = imu3000_queue.data[imu3000_queue.rp];
		imu3000_queue.rp = (imu3000_queue.rp + 1) % FIFO_SIZE;
		if(copy_to_user(buf, &data, sizeof(struct imu3000_data))) {
			up(&imu3000_dev.sem);
			return -EFAULT;
		}

		buf += sizeof(struct imu3000_data);
		copied += sizeof(struct imu3000_data);
		count -= sizeof(struct imu3000_data);
	}

	up(&imu3000_dev.sem);

	return copied;
}

static int imu3000_open(struct inode *inode, struct file *filp)
{
	if(down_killable(&imu3000_dev.sem))
		return -ERESTARTSYS;

	if(imu3000_num_readers == 0)
		imu3000_queue.wp = imu3000_queue.rp = 0;

	imu3000_num_readers++;

	up(&imu3000_dev.sem);
	return 0;
}

static int imu3000_release(struct inode *inode, struct file *filp)
{
	if(down_killable(&imu3000_dev.sem))
		return -ERESTARTSYS;

	imu3000_num_readers--;

	up(&imu3000_dev.sem);
	return 0;
}

static unsigned int imu3000_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;

	if(down_interruptible(&imu3000_dev.sem))
		return POLLHUP;
	poll_wait(filp, &imu3000_wait_queue, wait);
	if(imu3000_queue.wp != imu3000_queue.rp)
		mask |= POLLIN | POLLRDNORM;
	up(&imu3000_dev.sem);
	return mask;
}

static const struct file_operations imu3000_fops = {
	.owner = THIS_MODULE,
	.open = imu3000_open,
	.release = imu3000_release,
	.read = imu3000_read,
	.poll = imu3000_poll,
};

static int __init imu3000_init(void)
{
	int ret;

	ret = imu3000_init_irq();
	if(ret)
		return ret;

	ret = imu3000_init_i2c();
	if(ret)
		goto fail_5;

	memset(&imu3000_dev, 0, sizeof(struct imu3000_dev));

	sema_init(&imu3000_dev.sem, 1);

	ret = alloc_chrdev_region(&imu3000_dev.devt, 0, 1, "imu3000");
	if(ret < 0) {
		printk(KERN_WARNING "imu3000: alloc_chrdev_region() failed\n");
		goto fail_4;
	}

	cdev_init(&imu3000_dev.cdev, &imu3000_fops);
	imu3000_dev.cdev.owner = THIS_MODULE;

	ret = cdev_add(&imu3000_dev.cdev, imu3000_dev.devt, 1);
	if(ret) {
		printk(KERN_WARNING "imu3000: cdev_add() failed\n");
		goto fail_3;
	}

	imu3000_dev.class = class_create(THIS_MODULE, "imu3000");
	if(!imu3000_dev.class) {
		ret = -1;
		printk(KERN_WARNING "imu3000: class_create() failed\n");
		goto fail_2;
	}

	if(!device_create(imu3000_dev.class, NULL, imu3000_dev.devt, NULL, "imu3000")) {
		ret = -1;
		printk(KERN_WARNING "imu3000: device_create() failed\n");
		goto fail_1;
	}

	return 0;

fail_1:
	class_destroy(imu3000_dev.class);
fail_2:
	cdev_del(&imu3000_dev.cdev);
fail_3:
	unregister_chrdev_region(imu3000_dev.devt, 1);
fail_4:
	imu3000_cleanup_i2c();
fail_5:
	imu3000_cleanup_irq();

	return ret;
}
module_init(imu3000_init);

static void __exit imu3000_exit(void)
{
	device_destroy(imu3000_dev.class, imu3000_dev.devt);
	class_destroy(imu3000_dev.class);
	cdev_del(&imu3000_dev.cdev);
	unregister_chrdev_region(imu3000_dev.devt, 1);
	imu3000_cleanup_i2c();
	imu3000_cleanup_irq();
}
module_exit(imu3000_exit);

MODULE_LICENSE("GPL");
