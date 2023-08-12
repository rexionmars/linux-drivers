cmake_minimum_required(VERSION 3.26)
project(linux_drivers)

set(CMAKE_CXX_STANDARD 14)

add_executable(linux_drivers
        cuda/stage1/cudaAddInt.cu
        drivers/1_simple_LKM/generic.c
        drivers/2_dev_nr/dev_nr.c
        drivers/3_read_write/read_write.c
        drivers/4_gpio_dirver/gpio_driver.c
        drivers/5_lcd_driver/gpio_driver.c
        drivers/6_pwm_driver/gpio_driver.c)
