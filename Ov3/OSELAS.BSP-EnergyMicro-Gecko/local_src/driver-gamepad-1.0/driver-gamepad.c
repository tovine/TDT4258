/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
//#include <linux/sched.h>?

#include <stdbool.h>

#include "efm32gg.h"

#define CLASS_NAME "TDT4258_inputdev"

dev_t DEV_ID;
struct cdev my_cdev;
struct class *cl;
char DEV_NAME[] = "gamepad";

/* function to set up GPIO mode and interrupts*/
int setupGPIO()
{
	// Request access to the required memory regions
	if(request_mem_region(GPIO_PA_BASE, 0x01C, DEV_NAME) == NULL)
		return -2; // error
	if(request_mem_region(GPIO_PC_BASE, 0x068, DEV_NAME) == NULL)
		return -1; // error
	
//  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO; /* enable GPIO clock*/ - Not needed as the kernel does this
	*GPIO_PA_CTRL = 2;  /* set high drive strength */
	*GPIO_PA_MODEH |= 0x05550000; /* set pins A12-14 as output, rest should not be touched (registers are 0 by default, so OR-equals will work) */

	*GPIO_PC_MODEL = 0x33333333; /*set pins  C0-7 as input with filter */
	*GPIO_PC_DOUT = 0xFF; /* pull up direction up*/

	/* Set interrupts for pins 0-7 */
	*GPIO_EXTIPSELL = 0x22222222; 
	*GPIO_EXTIFALL = 0xFF;
//  *GPIO_EXTIRISE = 0xFF;
	*GPIO_IEN = 0xFF;  /*Enable interrupts for pins 0-7*/
	return 0;
}
/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

/*user program opens the driver */
static int my_open(struct inode *inode, struct file *filp) {
	nonseekable_open(inode, filp); // Device is not seekable
	//TODO
	return 0;
}
/* user program closes the driver */
static int my_release(struct inode *inode, struct file *filp) {
	//TODO
	return 0;
}
/* user program reads from the driver */
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
	//TODO - this should return status of buttons
	return 0;
}
/* user program writes to the driver */
static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {
	//TODO - this should light the LEDs
	return 0;
}

static struct file_operations my_fops = {
.owner = THIS_MODULE,
.read = my_read,
.write = my_write,
.open = my_open,
.release = my_release
};

static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");
	// Allocate a char device
	// int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
	if (alloc_chrdev_region(&DEV_ID, 0, 1, DEV_NAME) < 0) goto fail_alloc;
	printk(KERN_DEBUG "Allocated chardev region\n");
	cdev_init(&my_cdev, &my_fops);
	
	cl = class_create(THIS_MODULE, CLASS_NAME);
	device_create(cl, NULL, DEV_ID, NULL, CLASS_NAME);

	switch (setupGPIO()) { // < 0 means error, cleanup and exit
		case -1:
			goto fail_gpio_alloc_1;
		case -2:
			goto fail_gpio_alloc_1;
		default:
			printk(KERN_INFO "GPIO allocation OK");
	}
	// Finally, add the cdev - this has to be done after all initialization is done
	if (cdev_add(&my_cdev, DEV_ID, 1) < 0) goto fail_cdev;
	printk(KERN_DEBUG "Chardev added with id %d - init complete\n", DEV_ID);
	return 0;

	// Fail handling - release all allocated resources
		release_mem_region(GPIO_PC_BASE, 0x068);
	fail_gpio_alloc_1:
		release_mem_region(GPIO_PA_BASE, 0x01C);
	
		printk(KERN_ERR "Failed to request GPIO memory space");
	fail_cdev:
		cdev_del(&my_cdev);
		unregister_chrdev_region(DEV_ID, 1);
	fail_alloc:
	// TODO: return error code
	printk(KERN_ERR "Something failed :-(\n");
	return -1;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	// Release memory
	release_mem_region(GPIO_PA_BASE, 0x01C);
	release_mem_region(GPIO_PC_BASE, 0x068);

	unregister_chrdev_region(DEV_ID, 1);
	printk("Short life for a small module...\n");
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

/* man pages:
alloc_chrdev
class_create - if failed, unregister alloc
device_create
add_device (cdev_init)
cdev_add

// Functions for inserting memory barriers
#include <asm/system.h>
void rmb(void);
void read_barrier_depends(void); // Probably not good to use
void wmb(void);
void mb(void);
*/
