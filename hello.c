#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>	   //kmalloc()
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/err.h>
#include <linux/device.h>  // create device file
#include <linux/kdev_t.h> //allocating major/minor number
#include <linux/fs.h>	  /*for the char drv support*/

#define WR_VALUE _IOW('a', 'a', int32_t *) 
#define RD_VALUE _IOR('a', 'b', int32_t *)
int32_t value = 0;

#define mem_size 4000 // Memory Size
char *kernel_buffer;
dev_t dev = 0;

static long ex07_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg);

int ex07_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "Opening kernel file.....");
	return 0;
}

ssize_t ex07_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Data reading from kernel file.....");
	// Copy the data from the kernel space to the user-space
	if (copy_to_user(buffer, kernel_buffer, mem_size))
	{
		pr_err("Data Read : Err!\n");
	}
	pr_info("Data Read : Done!\n");

	return mem_size;
}

ssize_t ex07_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Data writing to kernel file.....");
	// Copy the data to kernel space from the user-space
	if (copy_from_user(kernel_buffer, buffer, length))
	{
		pr_err("Data Write : Err!\n");
	}
	pr_info("Data Write : Done!\n");
	return length;
}

int ex07_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "Closing kernel file.....");
	return 0;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long ex07_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
	case WR_VALUE:
		if (copy_from_user(&value, (int32_t *)arg, sizeof(value)))
		{
			pr_err("Data Write : Err!\n");
		}
		pr_info("Value = %d\n", value);
		break;
	case RD_VALUE:
		if (copy_to_user((int32_t *)arg, &value, sizeof(value)))
		{
			pr_err("Data Read : Err!\n");
		}
		break;
	default:
		pr_info("Default\n");
		break;
	}
	return 0;
}

struct file_operations ex07_file_operation = {
	.owner = THIS_MODULE,
	.open = ex07_open,
	.read = ex07_read,
	.write = ex07_write,
	.unlocked_ioctl = ex07_ioctl,
	.release = ex07_close,

};

int simple_module_init(void)
{
	printk(KERN_ALERT "Initialization of char driver module........");

	register_chrdev(200, "Simple Char Drv", &ex07_file_operation);
	/*Allocating Major number*/
	// if ((alloc_chrdev_region(&dev, 0, 1, "Simple Char Drv")) < 0)
	// {
	// 	printk(KERN_INFO "Cannot allocate major number for device 1\n");
	// 	return -1;
	// }
	// printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
	/*Creating cdev structure*/
	// cdev_init(&etx_cdev, &ex07_file_operation);

	// printk(KERN_INFO "Kernel Module Inserted Successfully...\n");

	if ((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		pr_info("Cannot allocate memory in kernel\n");
	}
	strcpy(kernel_buffer, "Hello_World");
	return 0;
}

void simple_module_exit(void)
{
	printk(KERN_ALERT "Removing of char driver module........");
	unregister_chrdev(MAJOR(dev), "Simple Char Drv");
	printk(KERN_ALERT "Removed char driver module sucessfully........");
}

module_init(simple_module_init);
module_exit(simple_module_exit);

MODULE_LICENSE("GPL");