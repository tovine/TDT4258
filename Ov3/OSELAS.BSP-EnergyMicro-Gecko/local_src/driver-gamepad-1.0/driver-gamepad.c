/*
 * This is the Linux kernel module for handling I/O operations using the TDT4258 gamepad.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
//#include <linux/sched.h>?
#include <asm/uaccess.h>
#include <asm/io.h>

#include <stdbool.h>

#include "efm32gg.h"

#define CLASS_NAME	"gamepad"
#define PA_ALLOW_MASK	0x7000 // 0b01110000 00000000 - used to mask out the LED pins on PORT_A we can actually use
#define IRQ_GPIO_EVEN	17
#define IRQ_GPIO_ODD	18

void __iomem *mem_gpio_port_c, *mem_gpio_int;

dev_t DEV_ID;
struct cdev my_cdev;
struct class *cl;
char DEV_NAME[] = "gamepad";
struct fasync_struct* async;
char buttoncache = 0, *bcache_ptr;

/* function to set up GPIO mode and interrupts*/
int setupGPIO(void)
{
	// Request access to the required memory regions
	if(request_mem_region(GPIO_PA_BASE, 0x01C, DEV_NAME) == NULL)
		return -5; // error
	if(request_mem_region(GPIO_PC_BASE, 0x020, DEV_NAME) == NULL)
		return -4; // error
	if(request_mem_region(GPIO_INT_BASE, 0x020, DEV_NAME) == NULL)
		return -3; // error

	mem_gpio_port_c = ioremap_nocache(GPIO_PC_BASE, 0x020);
	if(mem_gpio_port_c == 0) {
		printk(KERN_ERR "Failed to remap GPIO port C\n");
		return -2; // error
	}
	mem_gpio_int = ioremap_nocache(GPIO_INT_BASE, 0x020);
	if(mem_gpio_int == 0) {
		printk(KERN_ERR "Failed to remap GPIO interrupts\n");
		return -1; // error
	}
	// Use void *ioremap_nocache(unsigned long phys_addr, unsigned long size); - from <asm/io.h>
	// And void iounmap(void * addr);
	// For read/write: unsigned int ioread8(void *addr); and void iowrite8(u8 value, void *addr); - 8 can be replaced by 16 and 32 for different bit widths

	// Port A R/W is handled using direct pointers, as the |= operation is used a lot for that port, and using remapped memory needs 3 times as many steps
	*GPIO_PA_CTRL = 2;		/* set high drive strength for LEDs*/
	*GPIO_PA_MODEH |= 0x05550000;	/* set pins A12-14 as output, rest should not be touched (registers are 0 by default, so OR-equals will work) */
	*GPIO_PA_DOUT |= PA_ALLOW_MASK; // LEDs should be off by default

	//*GPIO_PC_MODEL = 0x33333333;
	//*GPIO_PC_DOUT = 0xFF;
	iowrite32(0x33333333, mem_gpio_port_c + MODEL_OFFSET);	/*set pins  C0-7 as input with filter */
	iowrite32(0xFF, mem_gpio_port_c + DOUT_OFFSET);		/* pull up direction up*/

	/* Set interrupts for pins 0-7 */
	//*GPIO_EXTIPSELL = 0x22222222; 
	//*GPIO_EXTIFALL = 0xFF;
	iowrite32(0x22222222, mem_gpio_int + EXTIPSELL_OFFSET);
	iowrite32(0xFF, mem_gpio_int + EXTIFALL_OFFSET);
	//*GPIO_IEN = 0xFF;
	/*Enable interrupts for pins 0-7*/
	iowrite32(0xFF, mem_gpio_int + IEN_OFFSET);

	return 0;
}

static irq_handler_t button_handler(int irq, void *dev_id, struct pt_regs *regs) {
	//uint8_t keys;
	// Clear interrupt flags, so they don't fire again
	//*GPIO_IFC= 0xff;
	iowrite8(0xFF, mem_gpio_int + IFC_OFFSET);
	//keys= ~ioread8(mem_gpio_port_c + DIN_OFFSET);
	if(async) {
		buttoncache = ~ioread8(mem_gpio_port_c + DIN_OFFSET);		// Cache the current button state
		bcache_ptr = &buttoncache;
		kill_fasync(&async, SIGIO, POLL_IN);	// Notify the other applications that something happened
	}
	//printk(KERN_INFO "Value of keys pressed %d", keys);
	return (irq_handler_t) IRQ_HANDLED;
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
//	printk(KERN_INFO "Device file opened\n");
	return 0;
}
/* user program closes the driver */
static int my_release(struct inode *inode, struct file *filp) {
//	printk(KERN_INFO "Device file closed\n");
	return 0;
}
/* Asynchronous file operations */
static int fasync(int inode_num, struct file *filp, int mode) {
	return fasync_helper(inode_num, filp, mode, &async);
}
/* user program reads from the driver */
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
	uint8_t keys;
	keys = ~ioread8(mem_gpio_port_c + DIN_OFFSET);
	if(bcache_ptr != NULL) { // Button read after interrupt should be buffered
		keys = buttoncache;
		bcache_ptr = NULL;
	}
//	printk(KERN_INFO "Status of buttons: %d", keys);
	put_user((char) keys, buff);
	return 0;
}
/* user program writes to the driver */
static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {
	uint8_t leds = 0b01110000; // Default: all LEDs off (output low -> LED on)
	printk(KERN_INFO "Received data: %d\n", *buff);
	switch(buff[0]){ // Input is translated to binary numbers [0-7] and displayed on the three available LEDs
		case 1:
		case '1':
			leds = 0b00110000; //0b01000000; //0b00010000;
			break;
		case 2:
		case '2':
			leds = 0b01010000; //0b00100000; //0b00100000;
			break;
		case 3:
		case '3':
			leds = 0b00010000; //0b01100000; //0b00110000;
			break;
		case 4:
		case '4':
			leds = 0b01100000; //0b00010000; //0b01000000;
			break;
		case 5:
		case '5':
			leds = 0b00100000; //0b01010000; //0b01010000;
			break;
		case 6:
		case '6':
			leds = 0b01000000; //0b00110000; //0b01100000;
			break;
		case 7:
		case '7':
			leds = 0b00000000; //0b01110000;
			break;
		default:
			printk(KERN_INFO "Invalid input - supported range: 0-7, got %d (char: %c)", buff[0], buff[0]);
	}
	*GPIO_PA_DOUT &= ~(PA_ALLOW_MASK );
	*GPIO_PA_DOUT |= (PA_ALLOW_MASK & (leds << 8)); // Shift bits one byte to the left, because only the upper byte is used
	return count;
}

static struct file_operations my_fops = {
.owner = THIS_MODULE,
.read = my_read,
.write = my_write,
.open = my_open,
.release = my_release,
.fasync = fasync
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

	// Register IRQ handlers - use the same function as we don't differentiate between even and odd buttons
	if(request_irq(IRQ_GPIO_ODD, (irq_handler_t) button_handler, 0, DEV_NAME, NULL) != 0) goto fail_requestIRQ1;
	if(request_irq(IRQ_GPIO_EVEN, (irq_handler_t) button_handler, 0, DEV_NAME, NULL) != 0) goto fail_requestIRQ2;

	switch (setupGPIO()) { // < 0 means error, cleanup and exit
		case -1:
		case -2:
		case -3:
		case -4:
		case -5:
			goto fail_gpio_alloc;
		default:
			printk(KERN_INFO "GPIO allocation OK\n");
	}
	// Finally, add the cdev - this has to be done after all initialization is done
	if (cdev_add(&my_cdev, DEV_ID, 1) < 0) goto fail_cdev;
	printk(KERN_DEBUG "Chardev added with id %d - init complete\n", DEV_ID);
	return 0;

	// Fail handling - release all allocated resources
	fail_gpio_alloc:
		release_mem_region(GPIO_PC_BASE, 0x020);
		release_mem_region(GPIO_INT_BASE, 0x020);
		release_mem_region(GPIO_PA_BASE, 0x01C);
		iounmap(mem_gpio_port_c);
		iounmap(mem_gpio_int);
		printk(KERN_ERR "Failed to request GPIO memory space\n");
		free_irq(IRQ_GPIO_EVEN, NULL);	
	fail_requestIRQ2:
		free_irq(IRQ_GPIO_ODD, NULL);	
	fail_requestIRQ1:
		class_destroy(cl);
	fail_cdev:
		cdev_del(&my_cdev);
		unregister_chrdev_region(DEV_ID, 1);
	fail_alloc:
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
	iounmap(mem_gpio_port_c);
	iounmap(mem_gpio_int);
	release_mem_region(GPIO_PA_BASE, 0x01C);
	release_mem_region(GPIO_PC_BASE, 0x020);
	release_mem_region(GPIO_INT_BASE, 0x020);
	free_irq(IRQ_GPIO_EVEN, NULL);	
	free_irq(IRQ_GPIO_ODD, NULL);	
	class_destroy(cl);
	cdev_del(&my_cdev);
	unregister_chrdev_region(DEV_ID, 1);
	printk("Short life for a small module...\n");
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Input/Output driver for TDT4258 gamepad");
MODULE_LICENSE("GPL");
