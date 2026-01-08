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
    
    // The error_code from lgGpiochipOpen is a negative errno value
    // Convert to positive for comparison with standard errno values
    int actual_errno = -error_code;
    
    // Check common error codes returned by lgGpiochipOpen
    if (actual_errno == 13) {  // EACCES - Permission denied
        print_error("Failed to open GPIO chip: Permission denied\n");
        print_error("The GPIO device %s exists but you don't have permission to access it.\n\n", gpio_device);
        print_error("Please refer to the README.md for GPIO permission setup instructions,\n");
        print_error("or run this program with sudo (not recommended).\n");
    } else if (actual_errno == 16) {  // EBUSY - Device busy
        print_error("Failed to open GPIO chip: Device or resource busy\n");
        print_error("The GPIO device %s is already in use by another process.\n", gpio_device);
        print_error("Try closing other programs that may be using GPIO, or check for background services.\n");
    } else if (actual_errno == 2) {  // ENOENT - No such file or directory
        print_error("Failed to open GPIO chip: Device %s not found\n", gpio_device);
        print_error("Make sure the GPIO chip exists and the chip number is correct.\n");
    } else if (actual_errno == 19) {  // ENODEV - No such device
        print_error("Failed to open GPIO chip: No such device\n");
        print_error("The GPIO device %s does not exist or the driver is not loaded.\n", gpio_device);
    } else {
        // Fall back to access() checks for unknown errors
        if (access(gpio_device, F_OK) == 0) {
            // Device exists but we got an unexpected error
            print_error("Failed to open GPIO chip: Error code %d\n", actual_errno);
            print_error("The GPIO device %s exists but failed to open.\n", gpio_device);
            print_error("This may indicate a driver issue or incompatible hardware state.\n");
        } else {
            print_error("Failed to open GPIO chip: Device %s not found\n", gpio_device);
        }
    }
}
