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

void check_gpio_device_error(int chip_id) {
    char gpio_device[GPIO_DEVICE_PATH_SIZE];
    int written = snprintf(gpio_device, sizeof(gpio_device), "/dev/gpiochip%d", chip_id);
    
    if (written < 0 || written >= (int)sizeof(gpio_device)) {
        print_error("Failed to open GPIO chip: unable to format GPIO device path\n");
        return;
    }
    
    if (access(gpio_device, F_OK) == 0) {
        // Device exists, check if it's a permission issue
        if (access(gpio_device, R_OK | W_OK) != 0) {
            print_error("Failed to open GPIO chip: Permission denied\n");
            print_error("The GPIO device %s exists but you don't have permission to access it.\n", gpio_device);
            print_error("\n");
            print_error("To fix this, you need to configure GPIO permissions:\n");
            print_error("1. Create a gpio group: sudo groupadd gpio\n");
            print_error("2. Add your user to it: sudo usermod -aG gpio <your-username>\n");
            print_error("3. Create udev rule: sudo nano /etc/udev/rules.d/99-gpio.rules\n");
            print_error("   Add this line: SUBSYSTEM==\"gpio\", KERNEL==\"gpiochip*\", GROUP=\"gpio\", MODE=\"0660\"\n");
            print_error("4. Reload udev: sudo udevadm control --reload-rules && sudo udevadm trigger\n");
            print_error("5. Reboot or log out and back in\n");
            print_error("\n");
            print_error("Alternatively, run this program with sudo (not recommended for production).\n");
        } else {
            // Device exists and permissions are OK, but still failed to open
            // This could be due to device being in use, driver issues, etc.
            print_error("Failed to open GPIO chip: Device in use or driver error\n");
            print_error("The GPIO device %s exists and you have permissions, but it failed to open.\n", gpio_device);
            print_error("It may be in use by another process or there may be a driver issue.\n");
        }
    } else {
        print_error("Failed to open GPIO chip: Device %s not found\n", gpio_device);
    }
}
