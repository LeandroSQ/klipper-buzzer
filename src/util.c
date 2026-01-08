#include "util.h"
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define GPIO_DEVICE_PATH_SIZE 32

void print(const char* msg, ...) {
    #ifdef VERBOSE
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    #endif
}

void print_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}

void check_gpio_device_error(int chip_id, int error_code) {
    char gpio_device[GPIO_DEVICE_PATH_SIZE];
    int written = snprintf(gpio_device, sizeof(gpio_device), "/dev/gpiochip%d", chip_id);
    
    if (written < 0 || written >= (int)sizeof(gpio_device)) {
        print_error("Failed to open GPIO chip: unable to format GPIO device path\n");
        return;
    }
    
    // Check lgpio-specific error codes
    // error_code from lgGpiochipOpen is already negative
    if (error_code == -93) {  // LG_NO_PERMISSIONS
        print_error("Failed to open GPIO chip: Permission denied\n");
        print_error("The GPIO device %s exists but you don't have permission to access it.\n\n", gpio_device);
        print_error("Please refer to the README.md for GPIO permission setup instructions,\n");
        print_error("or run this program with sudo (not recommended).\n");
    } else if (error_code == -79) {  // LG_GPIO_BUSY
        print_error("Failed to open GPIO chip: Device or resource busy\n");
        print_error("The GPIO device %s is already in use by another process.\n", gpio_device);
        print_error("Try closing other programs that may be using GPIO, or check for background services.\n");
    } else if (error_code == -78) {  // LG_CANNOT_OPEN_CHIP
        print_error("Failed to open GPIO chip: Cannot open gpiochip\n");
        print_error("Unable to open %s. Check if the device exists and you have proper permissions.\n", gpio_device);
        print_error("Refer to README.md for GPIO permission setup instructions.\n");
    } else if (error_code == -89) {  // LG_BAD_GPIOCHIP
        print_error("Failed to open GPIO chip: Bad gpiochip\n");
        print_error("The chip number %d is invalid or out of range.\n", chip_id);
    } else if (error_code == -81) {  // LG_NOT_A_GPIOCHIP
        print_error("Failed to open GPIO chip: Not a gpiochip\n");
        print_error("The device %s is not a valid GPIO chip.\n", gpio_device);
    } else {
        // Unknown lgpio error code
        print_error("Failed to open GPIO chip: Error code %d\n", error_code);
        print_error("Unable to open %s.\n", gpio_device);
        print_error("Refer to lgpio documentation for error code details.\n");
    }
}
