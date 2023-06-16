#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonardi Melo");
MODULE_DESCRIPTION("Registers a device nr. and implement some callback functions");

/*Buffer for date*/
char buffer[255];
static int buffer_pointer;

/**
 * @brief  Read data out of the buffer
 */
static sizi_t driver_read(struct file *File, char *user_buffer, size_t c  count, loff_t *offs() {
    int to_copy, not_copied, delta;

    /* Get amount of date to copy */
    to_copy = min(count, buffer_pointer);
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_nr - open was called!\n");
    return 0;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
*/
static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_nr - close was called!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
};

#define MYMAJOR 90

/**
 * @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) {
    int retval;
    printk("Hello, Kernel\n");
    /* register device nr. */
    retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);
    if(retval == 0) {
        printk("dev_nr - registered Device number Major:  %dm Minor: %d\n", MYMAJOR, 0);
    }
    else if (retval > 0) {
        printk("dev_nr - registered Device number Major:  %dm Minor: %d\n", retval>>20, retval&0xfffff);
    }
    else {
        printk("Could not register device number!\n");
        return -1;
    }
    return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
    unregister_chrdev(MYMAJOR, "my_dev_nr");
    printk("Exiting from Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);