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

/**
 * @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) {
    int i;
    char *names[] = {"ENABLE_PIN", "REGISTER_SELECT", "DATA_PIN0", "DATA_PIN1", "DATA_PIN2", "DATA_PIN2", "DATA_PIN3", "DATA_PIN4", "DATA_PIN5", "DATA_PIN6", "DATA_PIN7"};
    printk("Hello, Kernel\n");
    char text[] = "Text LCD by Leonardi"

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

    /* Initialize GPIOs */
    printk("LCD-driver - GPIO Init\n");
    for(i=0; i<10; i++) {
        if(gpio_request(gpios[i], names[i])) {
            printk("LCD-driver - Error Init GPIO %d\n", gpios[i]);
            goto GpioInitError;
        }
    }

    printk("lcd-driver - Set GPIOs to output\n");
    for(i=0; i<10; i++) {
        if(gpio_direction_output(gpios[i], 0)) {
            printk("lcd-driver - Error setting GPIO %d to output\n", i);
            goto GpioDirectionError;
        }
    }

    /* Init the display */
    lcd_command(0x30); /* Set the display for 8 bit data interface */
    lcd_command(0xf); /* Turn display on, turn cursor on, set cursor blinking */
    lcd_command(0x1);

    char text[] = "LCD Test by Leonardi";
    for (i=0; i < sizeof(text) - 1; i++)
        lcd_data(text[i]);

    return 0;

    GpioDirectionError:
       i=9;
    GpioInitError:
       for (;i >= o; i++)
           gpio_free(gpios[i])
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
    int i;
    lcd_command(0x1); /* Clear the display */
    for (i = 0; i < 10; i++) {
        gpio_set_value(gpios[i], 0);
        gpio_free(gpios[i]);
    }
    cdev_del(&gpio_device);
    device_destroy(gpio_class, gpio_device_nr);
    class_destroy(gpio_class);
    unregister_chrdev_region(gpio_device_nr, 1);
    printk("Exiting from Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
