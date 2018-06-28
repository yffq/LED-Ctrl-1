#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define MAJOR_NUM 111

static struct class		*firstdrv_class;
static struct class_device	*firstdrv_class_dev;
static struct cdev		*first_drv; 

static volatile unsigned long *gpfcon = NULL;
static volatile unsigned long *gpfdat = NULL;

static int first_drv_open(struct inode *my_inode, struct file *my_file)
{
	/* default to be level 4*/
	printk("first_drv_open\n");
	/* GPF4, 5, 6 correspond respectively LED1, LED2, LED4 */
	/* reset */
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	/* set, 0x01 means output */
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	
	/* 0 corresponds lighted LED */
	*gpfdat &= ~ ((1<<4) | (1<<5) | (1<<6));

	return 0;
}

/*
static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	//printk("first_drv_write\n");

	copy_from_user(&val, buf, count); //	copy_to_user();

	if (val == 1)
	{
		// µãµÆ
		*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
	}
	else
	{
		// ÃðµÆ
		*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	}
	
	return 0;
}
*/

static int first_drv_release(struct inode *my_inode, struct file *my_file)
{
	printk("first_drv_release\n");
	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	return 0;
}

 static struct file_operations first_drv_fops=
 {
         .owner  =       THIS_MODULE,
         .open   =       first_drv_open,
         .release=       first_drv_release,
 };

static int __init first_drv_init(void)
{
        int err;
	dev_t devid;
	devid = MKDEV(MAJOR_NUM, 0);
	first_drv = cdev_alloc();
	cdev_init(first_drv, &first_drv_fops);
	err = cdev_add(first_drv, devid, 1);
 	if(err)
		return -1;

	/*old way
	int major;
	major = register_chrdev(0, "first_drv", &first_drv_fops); 
       */

	firstdrv_class = (struct class *)class_create(THIS_MODULE, "firstdrv");
	/* /dev/firstdrv */
	firstdrv_class_dev = (struct class_device *)class_device_create(firstdrv_class, NULL, devid, NULL, "firstdrv");
	/*physical address converted to logical address*/
	gpfcon = (volatile unsigned long *)ioremap(0x56000050,0x16);
	gpfdat = (volatile unsigned long *)ioremap(0x56000054,0x08);

	return 0;
}

static void __exit first_drv_exit(void)
{
	cdev_del(first_drv);

	/*old way
	unregister_chrdev(major, "first_drv");
	*/ 
	
	class_device_unregister(firstdrv_class_dev);
	class_destroy(firstdrv_class);
	iounmap(gpfcon);
}

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LED Controller");
MODULE_VERSION("1.00");
