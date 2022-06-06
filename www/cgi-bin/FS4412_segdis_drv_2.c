
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/atomic.h>

#include "FS4412_segdis_drv_2.h"

#define DEV_NAME ("FS4412_segdis")

#define SEGDIS_BASEMINOR 0
#define SEGDIS_COUNT 1

MODULE_AUTHOR("Xiao Maolv");
MODULE_DESCRIPTION("segment displays drv for Ex3");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");

struct FS4412_segdis_struct 
{
	dev_t cd;
	atomic_t count;
	struct cdev Sdev;
	struct i2c_client *client;
	struct class *cls;
	struct device *dev;
};

static int __init FS4412_segdis_init(void);
static void __exit FS4412_segdis_exit(void);

static int __devinit FS4412_segdis_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __devexit FS4412_segdis_remove(struct i2c_client *client);

static int FS4412_segdis_open(struct inode *inode, struct file *filp);
static int FS4412_segdis_release(struct inode *inode, struct file *filp);
static ssize_t FS4412_segdis_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos);
static int FS4412_segdis_w_cmd(char *buffer, const size_t count);

struct FS4412_segdis_struct FS4412_segdis;

static const struct i2c_device_id FS4412_segdis_idtbl[] = 
{
	{DEV_NAME, 0},
	{ },
};
MODULE_DEVICE_TABLE(i2c, FS4412_segdis_idtbl);

static struct i2c_driver FS4412_segdis_driver = 
{
	.probe = FS4412_segdis_probe,
	.remove = __devexit_p(FS4412_segdis_remove),
	.id_table = FS4412_segdis_idtbl,
	.driver	= 
	{
		.name	= DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = FS4412_segdis_open,
	.release = FS4412_segdis_release,
	.write = FS4412_segdis_write,
};

static int __init FS4412_segdis_init(void)
{
	/* register segment display driver */
	return i2c_add_driver(&FS4412_segdis_driver);
}

static void __exit FS4412_segdis_exit(void)
{
	/* unregister segment display driver */
	i2c_del_driver(&FS4412_segdis_driver);
}

static int __devinit FS4412_segdis_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err;
	
	FS4412_segdis.client = client;
	
	/* allocate device numbers to segment display */
	err = alloc_chrdev_region(&FS4412_segdis.cd, SEGDIS_BASEMINOR, SEGDIS_COUNT, DEV_NAME);
	if (err < 0)
	{
		printk(KERN_ERR "fail to allocate segdis device number\n");
		return err;
	}

	/* initialize cdev structure of segment display */
	cdev_init(&FS4412_segdis.Sdev, &fops);
	FS4412_segdis.Sdev.owner = THIS_MODULE;

	/* add segment display device to the system */
	err = cdev_add(&FS4412_segdis.Sdev, FS4412_segdis.cd, SEGDIS_COUNT);
	if (err < 0)
	{
		printk(KERN_ERR "fail to add segdis to system\n");
		unregister_chrdev_region(FS4412_segdis.cd, SEGDIS_COUNT);
		return err;
	}

	/* create segment display device */
	FS4412_segdis.cls = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(FS4412_segdis.cls))
	{
		printk(KERN_ERR "fail to create class");
		return PTR_ERR(FS4412_segdis.cls);
	}
	FS4412_segdis.dev = device_create(FS4412_segdis.cls, NULL, FS4412_segdis.cd, NULL, DEV_NAME);
	if (IS_ERR(FS4412_segdis.dev))
	{
		printk(KERN_ERR "fail to create device");
		return PTR_ERR(FS4412_segdis.dev);
	}

	printk(KERN_NOTICE "insert segdis drv\n");
	return 0;
}

static int __devexit FS4412_segdis_remove(struct i2c_client *client)
{
	/* remove segment display device */
	device_destroy(FS4412_segdis.cls, FS4412_segdis.cd);
	class_destroy(FS4412_segdis.cls);
	/* remove segment display from the system */
	cdev_del(&FS4412_segdis.Sdev);
	/* unregister device numbers of segment display */
	unregister_chrdev_region(FS4412_segdis.cd, SEGDIS_COUNT);
	printk(KERN_NOTICE "remove segdis drv\n");
	return 0;
}

static int FS4412_segdis_open(struct inode *inode, struct file *filp)
{
	/* to be exclusive */
	if (atomic_read(&FS4412_segdis.count))
	{
		printk(KERN_ERR "segment display busy\n");
		return -EBUSY;
	}
	atomic_add(1, &FS4412_segdis.count);

	/* clear segment display*/
	char buf[] = {ZLG7290_CMDBUF0_REG_ADDR, ZLG7290_COMBUF0_SL_PREF | ZLG7290_COMBUF0_SL_VAL};
	int err = i2c_master_send(FS4412_segdis.client, buf, 2);
	if (err < 0)
	{
		printk(KERN_ERR "fail to clear the display when open\n");
		return err;
	}
	return 0;
}

static int FS4412_segdis_release(struct inode *inode, struct file *filp)
{
	/* clear segment display*/
	char buf[] = {ZLG7290_CMDBUF0_REG_ADDR, ZLG7290_COMBUF0_SL_PREF | ZLG7290_COMBUF0_SL_VAL};
	int err = i2c_master_send(FS4412_segdis.client, buf, 2);
	if (err < 0)
	{
		printk(KERN_ERR "fail to clear the display when close\n");
		return err;
	}

	if (!atomic_read(&FS4412_segdis.count))
	{
		printk(KERN_ERR "device closed before");
		return -ENODEV;
	}
	atomic_sub(1, &FS4412_segdis.count);
	return 0;
}

static ssize_t FS4412_segdis_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos)
{
	uint8_t i;
	int err;
	char *wbuf;
	if (count != 2 && count != 9)
		goto inval_err;
	
	wbuf = kmalloc(count, GFP_KERNEL);
	if (!wbuf)
	{
		printk(KERN_ERR "fail to alloc mem for write buf\n");
		return -ENOMEM;
	}
	if (copy_from_user(wbuf, buffer, count))
	{
		printk(KERN_ERR "fail to get write buf from usr\n");
		goto copy_err;
	}
	
	if (wbuf[0] > 9)
		goto inval_err;
	else if (!wbuf[0] && wbuf[1] > 8)
		goto inval_err;
	else if (wbuf[0])
	{
		for (i = 1; i < count; i++)
		{
			if (wbuf[i] >= 'A' && wbuf[i] <= 'F')
				wbuf[i] = wbuf[i] - 'A' + 10;
			else if (wbuf[i] >= 'a' && wbuf[i] <= 'f')
				wbuf[i] = wbuf[i] - 'a' + 10;
			else if (wbuf[i] >= '0' && wbuf[i] <= '9')
				wbuf[i] = wbuf[i] - '0';
			else
				goto inval_err;
		}
	}
	
	err = FS4412_segdis_w_cmd(wbuf, count);
	if (err < 0)
	{
		printk(KERN_ERR "i2c write err\n");
		return err;
	}
	
	kfree(wbuf);
	return 0;

inval_err:
	printk(KERN_ERR "invalid agrs\n");
copy_err:
	kfree(wbuf);
	return -EINVAL;
}

/**********************************************************************
 * @Function:	FS4412_segdis_w_cmd
 * @Description:	issue segment display control command
 * @Param[in]:	(char)buffer[]: command
 * 				(size_t)count: length of buffer
 * @Return:	0 if success, otherwise negative errno
 **********************************************************************/
static int FS4412_segdis_w_cmd(char *buffer, const size_t count)
{
	int i;
	char buf[9] = { ZLG7290_DPRAM0_ADDR };
	uint8_t reg_addr[] = { ZLG7290_DPRAM0_ADDR, ZLG7290_DPRAM1_ADDR,ZLG7290_DPRAM2_ADDR,
							ZLG7290_DPRAM3_ADDR, ZLG7290_DPRAM4_ADDR, ZLG7290_DPRAM5_ADDR,
								ZLG7290_DPRAM6_ADDR, ZLG7290_DPRAM7_ADDR };
	uint8_t num_tbl[] = { SEG_DIS_0, SEG_DIS_1, SEG_DIS_2, SEG_DIS_3, SEG_DIS_4, 
							SEG_DIS_5, SEG_DIS_6, SEG_DIS_7, SEG_DIS_8, SEG_DIS_9, 
								SEG_DIS_A, SEG_DIS_B, SEG_DIS_C, SEG_DIS_D, SEG_DIS_E, 
									SEG_DIS_F };

	switch (buffer[0])
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		buf[0] = reg_addr[buffer[0] - 1];
		buf[1] = num_tbl[(int)buffer[1]];
		return i2c_master_send(FS4412_segdis.client, buf, 2);
	case 9:
		for (i = 1; i <= 8; i++)
			buf[i] = num_tbl[(int)buffer[i]];
	}

	return i2c_master_send(FS4412_segdis.client, buf, 9);
}

module_init(FS4412_segdis_init);
module_exit(FS4412_segdis_exit);
