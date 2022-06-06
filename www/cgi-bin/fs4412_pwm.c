#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "fs4412_pwm.h"

MODULE_LICENSE("GPL");

#define EXYNOS4412_GPD0CON  	0x114000A0
#define EXYNOS4412_TIMER_BASE  	0x139D0000

#define TCFG0	0x00
#define TCFG1	0x04
#define TCON	0x08
#define TCNTB0	0x0C
#define TCMPB0	0x10

#define PWM_MAJOR 501
#define PWM_MINOR 0
#define PWM_NUM	  1

struct pwm_device 
{
	struct cdev cdev;	
	void __iomem *gpd0con;
	void __iomem *timer_base;
};

static struct pwm_device *pwm_dev;

static int fs4412_pwm_open(struct inode *inode, struct file *file)
{
	writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
	writel((readl(pwm_dev->timer_base + TCFG0) & ~0xff) | 0x1,pwm_dev->timer_base + TCFG0);
	writel(readl(pwm_dev->timer_base + TCFG1) & ~0xf ,pwm_dev->timer_base + TCFG1);
	writel(0xF, pwm_dev->timer_base + TCNTB0);
	writel(0x0, pwm_dev->timer_base + TCMPB0);
	writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0x2 ,pwm_dev->timer_base + TCON);
//	writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0x9 ,pwm_dev->timer_base + TCON);
	return 0;
}

static int fs4412_pwm_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long fs4412_pwm_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int nr;

	if(copy_from_user((void *)&nr, (void *)arg, sizeof(nr)))
		return -EFAULT;

	switch(cmd) {
	case PWM_ON:
		writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0xD ,pwm_dev->timer_base + TCON);
		break;
	case PWM_OFF:
		writel(readl(pwm_dev->timer_base + TCON) & ~0xf ,pwm_dev->timer_base + TCON);
		break;
	case SET_PRE:
		break;
	case SET_CNT:
		writel(0xF, pwm_dev->timer_base + TCNTB0);
		writel(0x7 - (nr & 0x7), pwm_dev->timer_base + TCMPB0);
		break;
	default:
		printk("Invalid argument\n");
		return -EINVAL;
	}

	return 0;	
}

static struct file_operations fs4412_pwm_fops = {
	.owner = THIS_MODULE,
	.open = fs4412_pwm_open,
	.release =fs4412_pwm_release,
	.unlocked_ioctl = fs4412_pwm_unlocked_ioctl,
}; 

static int fs4412_pwm_ioremap(void) {
	int ret;

	pwm_dev->gpd0con = ioremap(EXYNOS4412_GPD0CON, 4);		
	if (pwm_dev->gpd0con == NULL) {
		ret = -ENOMEM;
		return ret;
	}

	pwm_dev->timer_base = ioremap(EXYNOS4412_TIMER_BASE, 0x20);		
	if (pwm_dev->timer_base == NULL) {
		ret = -ENOMEM;
		goto out1;
	}

	return 0;

out1:
	iounmap(pwm_dev->gpd0con);
	return ret;
}

void fs4412_pwm_iounmap(void)
{
	iounmap(pwm_dev->gpd0con);
	iounmap(pwm_dev->timer_base);
}

static int fs4412_pwm_init(void)
{
	int ret;
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);
	
	printk("pwm init\n");

	ret = register_chrdev_region(devno, PWM_NUM, "pwm");
	if (ret < 0) {
		printk("failed to register_chrdev_region\n");
		return ret;
	}

	pwm_dev = kzalloc(sizeof(*pwm_dev), GFP_KERNEL);
	if (pwm_dev == NULL) {
		ret = -ENOMEM;
		printk("failed to kzalloc\n");
		goto err1;
	}

	cdev_init(&pwm_dev->cdev, &fs4412_pwm_fops);
	pwm_dev->cdev.owner = THIS_MODULE;
	ret = cdev_add(&pwm_dev->cdev, devno, PWM_NUM);
	if (ret < 0) {
		printk("failed to cdev_add\n");
		goto err2;
	}

	ret = fs4412_pwm_ioremap();
	if (ret < 0) {
		printk("failed to ioremap\n");
		goto err3;
	}

	return 0;

err3:
	cdev_del(&pwm_dev->cdev);
err2:
	kfree(pwm_dev);
err1:
	unregister_chrdev_region(devno, PWM_NUM);
	return ret;
}

static void fs4412_pwm_exit(void)
{
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);
	fs4412_pwm_iounmap();
	cdev_del(&pwm_dev->cdev);
	kfree(pwm_dev);
	unregister_chrdev_region(devno, PWM_NUM);
	printk("pwm exit\n");
}

module_init(fs4412_pwm_init);
module_exit(fs4412_pwm_exit);
