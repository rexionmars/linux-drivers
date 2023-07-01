#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("João Leonardi da Silva Melo");
MODULE_DESCRIPTION("Driver to write to a LCD text display");

/* Variables for device and device class */
static dev_t lcd_device_nr;
static struct class *lcd_class;
static struct cdev lcd_device;

#define DRIVER_NAME "rpi_lcd_driver"
#define DRIVER_CLASS "LCDModuleClass"

/* LCD char buffer */
static char lcd_buffer[17];

/* Pinout for LCD Display */
unsigned int gpios[] = {
    3,  /* Enable Pin */
    2,  /* Register Select Pin */
    4,  /* Data Pin 0 */
    5,  /* Data Pin */
    9,  /* Data Pin 5 */
    10, /* Data Pin 4 */
    11, /* Data Pin 6 */
    17, /* Data Pin 1 */
    22, /* Data Pin 3 */
    27, /* Data Pin 2 */
};

#define ENABLE_PIN gpios[0]
#define REGISTER_SELECT gpios[1]

/**
 * @brief Write data to buffer
*/
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta, i;

    /* Get amount of date to copy */
    to_copy = min(count, sizeof(lcd_buffer));

    /* Copy data to user */
    not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);

    /* Calculate data */
    delta = to_copy - not_copied;

    return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_nr - open was called!\n");
    return 0;
}

/**
 * @brief This function is called, when the device  file is close
*/
static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_nr - close was called!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write,
};

#define MYMAJOR 90

/**
 * @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) {
    int retval;
    printk("Hello, Kernel\n");

    /* Allocate a device nr */
    if(alloc_chrdev_region(&gpio_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", gpio_device_nr >> 20, gpio_device_nr && 0xfffff);

    /* Create device class */
    if((gpio_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not e created!\n");
        goto ClassError;
    }

    /* Create device file */
    if(device_create(gpio_class, NULL, gpio_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Can not created device file!\n");
        goto FileError;
    }

    /* Initialize device file */
    cdev_init(&gpio_device, &fops);

    /* Registering device to kernel */
    if(cdev_add(&gpio_device, gpio_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }

    /* GPIO 4 init */
    if(gpio_request(4, "rpi-gpio-4")) {
        printk("Can not allocate GPIO 4\n");
        goto AddError;
    }

    /* Set GPIO 4 direction */
    if(gpio_direction_output(4, 0)) {
        printk("Can not set GPIO 4 to output!\n");
        goto Gpio4Error;
    }

    /* GPIO 17 init */
    if(gpio_request(17, "rpi-gpio-17")) {
        printk("Can not allocate GPIO 17\n");
        goto Gpio4Error;
    }

    /* Set GPIO 17 direction */
    if(gpio_direction_input(17)) {
        printk("Can not set GPIO 17 to input!\n");
        goto Gpio17Error;
    }

    return 0;

    Gpio17Error:
       gpio_free(17);
    Gpio4Error:
       gpio_free(4);
    AddError:
       device_destroy(gpio_class, gpio_device_nr);
    FileError:
       class_destroy(gpio_class);
    ClassError:
       unregister_chrdev(gpio_device_nr, DRIVER_NAME);

    return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
    gpio_set_value(4, 0);
    gpio_free(17);
    gpio_free(4);
    cdev_del(&gpio_device);
    device_destroy(gpio_class, gpio_device_nr);
    class_destroy(gpio_class);
    unregister_chrdev(gpio_device_nr, DRIVER_NAME);
    printk("Exiting from Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
