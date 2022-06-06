#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "fs4412_pwm.h"

MODULE_LICENSE("GPL");  //ģ������֤���� 

#define EXYNOS4412_GPD0CON  	0x114000A0  //motor��pwm��ӦI/O������ 
#define EXYNOS4412_TIMER_BASE  	0x139D0000  //timer 0����ʼ��ַ 

#define TCFG0	0x00  //��ʱ�����üĴ���0
#define TCFG1	0x04  //��ʱ�����üĴ���1
#define TCON	0x08  //��ʱ�����ƼĴ���
#define TCNTB0	0x0C  //��������Ĵ���
#define TCMPB0	0x10  //�Ƚϻ���Ĵ���

#define PWM_MAJOR 501  //���豸�� 
#define PWM_MINOR 0  //���豸�� 
#define PWM_NUM	  1  //PWM�豸���� 

struct pwm_device 
{
	struct cdev cdev;	
	void __iomem *gpd0con;  //__iomemָ��������ʾָ��һ��I/O���ڴ�ռ䣬void��ʾ����������ԶԱ����ļ�顣��Ҫ�������м�飬��__iomem��ָ���������ָ�����ʱ���ͻᷢ��һЩ����
	void __iomem *timer_base;
};

static struct pwm_device *pwm_dev;

//timer 0�ĳ�ʼ�������������޷�ֱ�Ӳ��������ַ����Ҫ�������ַת���������ַ 
static int fs4412_pwm_open(struct inode *inode, struct file *file)
{
	//�����ݴӼĴ����ж������޸ĺ���д�ؼĴ���
	writel((readl(pwm_dev->gpd0con) & ~0xf) | 0x2,pwm_dev->gpd0con);
	writel((readl(pwm_dev->timer_base + TCFG0) & ~0xff) | 0x1,pwm_dev->timer_base + TCFG0);
	writel(readl(pwm_dev->timer_base + TCFG1) & ~0xf ,pwm_dev->timer_base + TCFG1);
	
	//װ�����ݣ�����ռ�ձ�
	writel(0xF, pwm_dev->timer_base + TCNTB0);
	writel(0x0, pwm_dev->timer_base + TCMPB0);
	writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0x2 ,pwm_dev->timer_base + TCON);  //timer 0�ֶ����� 

	return 0;
}

//�ر��豸 
static int fs4412_pwm_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long fs4412_pwm_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int nr;
	
	//copy_from_user����ʵ�ִ��û��ռ俽�����ݵ��ں˿ռ䣬ʧ�ܷ���û�б��������ֽ������ɹ�����0
	//��arg��ַ�е����ݿ�����nr�ں˵�ַ��ȥ����������Ϊsizeof(nr) 
	if(copy_from_user((void *)&nr, (void *)arg, sizeof(nr)))  
		return -EFAULT;

	switch(cmd) {
	case PWM_ON:
		//timer 0�����������ת���Զ����� 
		writel((readl(pwm_dev->timer_base + TCON) & ~0xf) | 0xD ,pwm_dev->timer_base + TCON);
		break;
	case PWM_OFF:
		//ֹͣ
		writel(readl(pwm_dev->timer_base + TCON) & ~0xf ,pwm_dev->timer_base + TCON);
		break;
	case SET_PRE:
		break;
	case SET_CNT:
		//�ı��ٶ� 
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

//file_operation�ṹ�����豸�����ļ��ṹ�壬Ӳ���豸�ӿڲ��������������������豸�Ľ�����ͨ�������ļ�ϵͳ���豸��������Ľӿ�ʵ�֡�
static struct file_operations fs4412_pwm_fops = {
	.owner = THIS_MODULE,
	.open = fs4412_pwm_open,
	.release =fs4412_pwm_release,
	.unlocked_ioctl = fs4412_pwm_unlocked_ioctl,
}; 

//ioremap����ʵ�ֵ�ַӳ����� 
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

//iounmap��������ȡ��ioremap������ӳ��
void fs4412_pwm_iounmap(void)
{
	iounmap(pwm_dev->gpd0con);
	iounmap(pwm_dev->timer_base);
}

static int fs4412_pwm_init(void)
{
	int ret;
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);  //MKDEV�����豸�źʹ��豸��ת����dev_t���� 
	
	printk("pwm init\n");
	
	//��̬�����豸�� 
	ret = register_chrdev_region(devno, PWM_NUM, "pwm"); 
	if (ret < 0) {
		printk("failed to register_chrdev_region\n");
		return ret;
	}
	
	//�����ڴ�ռ䣬�����ڴ滺���� 
	pwm_dev = kzalloc(sizeof(*pwm_dev), GFP_KERNEL);
	if (pwm_dev == NULL) {
		ret = -ENOMEM;
		printk("failed to kzalloc\n");
		goto err1;
	}
	
	cdev_init(&pwm_dev->cdev, &fs4412_pwm_fops);  //��ʼ���ַ��豸���� 
	pwm_dev->cdev.owner = THIS_MODULE;  //��ʼ���豸������
	ret = cdev_add(&pwm_dev->cdev, devno, PWM_NUM);  //���ַ��豸������뵽�ں�ϵͳ 
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
	cdev_del(&pwm_dev->cdev);  //���ں�ϵͳ��ɾ�����ַ��豸���� 
err2: 
	kfree(pwm_dev);  //�ͷ���kmalloc����������ڴ滺���� 
err1:
	unregister_chrdev_region(devno, PWM_NUM);  //�ջ��豸�� 
	return ret;
}

static void fs4412_pwm_exit(void)
{
	dev_t devno = MKDEV(PWM_MAJOR, PWM_MINOR);
	fs4412_pwm_iounmap();
	cdev_del(&pwm_dev->cdev);  //���ں��н��豸ɾ��
	kfree(pwm_dev);
	unregister_chrdev_region(devno, PWM_NUM);  //�ջ��豸�� 
	printk("pwm exit\n");
}

module_init(fs4412_pwm_init);  //ģ�����
module_exit(fs4412_pwm_exit);  //ģ��ж��
