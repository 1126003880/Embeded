#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "fs4412_pwm.h"

MODULE_LICENSE("GPL");  //模块的许可证声明 

#define EXYNOS4412_GPD0CON  	0x114000A0  //motor—pwm对应I/O的引脚 
#define EXYNOS4412_TIMER_BASE  	0x139D0000  //timer 0的起始地址 

#define TCFG0	0x00  //定时器配置寄存器0
#define TCFG1	0x04  //定时器配置寄存器1
#define TCON	0x08  //定时器控制寄存器
#define TCNTB0	0x0C  //计数缓冲寄存器
#define TCMPB0	0x10  //比较缓冲寄存器

#define PWM_MAJOR 501  //主设备号 
#define PWM_MINOR 0  //次设备号 
#define PWM_NUM	  1  //PWM设备数量 

struct pwm_device 
{
	struct cdev cdev;	
	void __iomem *gpd0con;  //__iomem指针用来表示指向一个I/O的内存空间，void表示编译器会忽略对变量的检查。若要对它进行检查，当__iomem的指针和正常的指针混用时，就会发出一些警告
	void __iomem *timer_base;
};

static struct pwm_device *pwm_dev;

//timer 0的初始化操作，驱动无法直接操纵物理地址，需要将物理地址转换成虚拟地址 
static int fs4412_pwm_open(struct inode *inode, struct file *file)
{
	//将数据从寄存器中读出，修改后再写回寄存器
	writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
	writel((readl(pwm_dev->timer_base + TCFG0) & ~0xff) | 0x1,pwm_dev->timer_base + TCFG0);
	writel(readl(pwm_dev->timer_base + TCFG1) & ~0xf ,pwm_dev->timer_base + TCFG1);
	
	//装载数据，配置占空比
	writel(0xF, pwm_dev->timer_base + TCNTB0);
	writel(0x0, pwm_dev->timer_base + TCMPB0);
	writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0x2 ,pwm_dev->timer_base + TCON);  //timer 0手动更新 

	return 0;
}

//关闭设备 
static int fs4412_pwm_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long fs4412_pwm_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int nr;
	
	//copy_from_user函数实现从用户空间拷贝数据到内核空间，失败返回没有被拷贝的字节数，成功返回0
	//将arg地址中的数据拷贝到nr内核地址中去，拷贝长度为sizeof(nr) 
	if(copy_from_user((void *)&nr, (void *)arg, sizeof(nr)))  
		return -EFAULT;

	switch(cmd) {
	case PWM_ON:
		//timer 0开启，输出翻转，自动重载 
		writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0xD ,pwm_dev->timer_base + TCON);
		break;
	case PWM_OFF:
		//停止
		writel(readl(pwm_dev->timer_base + TCON) & ~0xf ,pwm_dev->timer_base + TCON);
		break;
	case SET_PRE:
		break;
	case SET_CNT:
		//改变速度 
		if(nr==0)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x4,pwm_dev->gpd0con);	
		}
		if(nr==1)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x1 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x6, pwm_dev->timer_base + TCMPB0);
		 }
		 if(nr==2)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x2 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x5, pwm_dev->timer_base + TCMPB0);
		 } 
		 if(nr==3)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x3 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x4, pwm_dev->timer_base + TCMPB0);
		 } 
		 if(nr==4)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x4 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x3, pwm_dev->timer_base + TCMPB0);
		 } 
		 if(nr==5)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x5 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x2, pwm_dev->timer_base + TCMPB0);
		 } 
		 if(nr==6)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x6 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x1, pwm_dev->timer_base + TCMPB0);
		 } 
		 if(nr==7)
		{
			writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
			writel(0xF, pwm_dev->timer_base + TCNTB0);
			writel(0x7 - (0x7 & 0x7), pwm_dev->timer_base + TCMPB0);
//			writel(0x0, pwm_dev->timer_base + TCMPB0);
		 } 

		break;
	default:
		printk("Invalid argument\n");
		return -EINVAL;
	}

	return 0;	
}

//file_operation结构定义设备驱动文件结构体，硬件设备接口层用来描述驱动程序与设备的交互，通过虚拟文件系统与设备驱动程序的接口实现。
static struct file_operations fs4412_pwm_fops = {
	.owner = THIS_MODULE,
	.open = fs4412_pwm_open,
	.release =fs4412_pwm_release,
	.unlocked_ioctl = fs4412_pwm_unlocked_ioctl,
}; 

//ioremap函数实现地址映射操作 
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

//iounmap函数用于取消ioremap所做的映射
void fs4412_pwm_iounmap(void)
{
	iounmap(pwm_dev->gpd0con);
	iounmap(pwm_dev->timer_base);
}

static int fs4412_pwm_init(void)
{
	int ret;
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);  //MKDEV将主设备号和次设备号转换成dev_t类型 
	
	printk("pwm init\n");
	
	//静态申请设备号 
	ret = register_chrdev_region(devno, PWM_NUM, "pwm"); 
	if (ret < 0) {
		printk("failed to register_chrdev_region\n");
		return ret;
	}
	
	//申请内存空间，开辟内存缓冲区 
	pwm_dev = kzalloc(sizeof(*pwm_dev), GFP_KERNEL);
	if (pwm_dev == NULL) {
		ret = -ENOMEM;
		printk("failed to kzalloc\n");
		goto err1;
	}
	
	cdev_init(&pwm_dev->cdev, &fs4412_pwm_fops);  //初始化字符设备对象 
	pwm_dev->cdev.owner = THIS_MODULE;  //初始化设备持有者
	ret = cdev_add(&pwm_dev->cdev, devno, PWM_NUM);  //将字符设备对象加入到内核系统 
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
	cdev_del(&pwm_dev->cdev);  //从内核系统中删除的字符设备对象 
err2: 
	kfree(pwm_dev);  //释放由kmalloc分配出来的内存缓冲区 
err1:
	unregister_chrdev_region(devno, PWM_NUM);  //收回设备号 
	return ret;
}

static void fs4412_pwm_exit(void)
{
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);
	fs4412_pwm_iounmap();
	cdev_del(&pwm_dev->cdev);  //从内核中将设备删除
	kfree(pwm_dev);
	unregister_chrdev_region(devno, PWM_NUM);  //收回设备号 
	printk("pwm exit\n");
}

module_init(fs4412_pwm_init);  //模块加载
module_exit(fs4412_pwm_exit);  //模块卸载
