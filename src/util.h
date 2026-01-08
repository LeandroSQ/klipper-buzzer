#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>

void print(const char* msg, ...);
void print_error(const char* msg, ...);
void check_gpio_device_error(int chip_id);

#endif