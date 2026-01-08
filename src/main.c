#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "util.h"
#include "parser.h"
#ifdef  __APPLE__
    #include "include/lgpio.h"
#else
    #include <lgpio.h>
#endif

const uint8_t GPIO_PIN = 64;
const uint8_t GPIO_CHIP = 0;
const char* SONGS_FOLDER = __FILE__ "/songs";
const int GPIO_DEVICE_PATH_SIZE = 32;

int main(int argc, char **argv) {
    // Arguments:
    // <filename> or none
    if (argc > 2) {
        print_error("Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    // Get the filename, we need to 'strdup' it because the parser will free it
    char* filename = argc == 2 ? strdup(argv[1]) : NULL;
    bool isDirectory = false;

    // Check if the filename is actually a folder
    if (filename != NULL) {
        struct stat s;
        if (stat(filename, &s) == 0 && S_ISDIR(s.st_mode)) {
            print("Selected folder: %s\n", filename);
            isDirectory = true;
        }
    }

    if (filename == NULL || isDirectory) {
        const char* folder = isDirectory ? filename : SONGS_FOLDER;
        filename = (char*)getRandomFileInFolder(folder);
        if (filename == NULL) {
            print_error("Failed to get random file in folder %s\n", folder);
            return 1;
        }

        print("Selected random file: %s\n", filename);
    } else {
        print("Selected file: %s\n", filename);
    }

    // Parse the melody
    Melody* melody = parseMelodyFile(filename);
    if (melody == NULL) {
        print_error("Failed to parse melody\n");
        free(filename);
        return 1;
    }

	int handle = lgGpiochipOpen(GPIO_CHIP);
	// lgGpiochipOpen returns a handle >= 0 on success, or a negative error code on failure
	if (handle < 0) {
        free(filename);
        free(melody->tones);
        free(melody);
        
        // Check if the GPIO device exists but we don't have permission
        char gpio_device[GPIO_DEVICE_PATH_SIZE];
        snprintf(gpio_device, sizeof(gpio_device), "/dev/gpiochip%d", GPIO_CHIP);
        
        if (access(gpio_device, F_OK) == 0) {
            // Device exists, check if it's a permission issue
            if (access(gpio_device, R_OK | W_OK) != 0) {
                print_error("Failed to open GPIO chip: Permission denied\n");
                print_error("The GPIO device %s exists but you don't have permission to access it.\n", gpio_device);
                print_error("Try running with sudo or add your user to the 'gpio' group:\n");
                print_error("  sudo usermod -aG gpio $USER\n");
                print_error("Then log out and log back in for the changes to take effect.\n");
            } else {
                print_error("Failed to open GPIO chip (error code: %d)\n", handle);
            }
        } else {
            print_error("Failed to open GPIO chip: Device %s not found\n", gpio_device);
        }
		return 1;
	} else {
        print("Opened GPIO chip!\n");
    }

    if (lgGpioClaimOutput(handle, 0, GPIO_PIN, 0) != LG_OKAY) {
        print_error("Failed to claim GPIO\n");
    } else {
        print("Claimed GPIO!\n");

        print("Melody has pitch: %d\n", melody->pitch);
        print("Melody has %d tones\n", melody->length);
        const float factor = powf(2.0f, melody->pitch / 12.0f);
        for (int i = 0; i < melody->length; i++) {
            Tone tone = melody->tones[i];
            print("Playing note: %d for %d ms\n", tone.note, tone.duration);

            const float durationInSeconds = tone.duration / 1000.0f;

            if (tone.note == 0) {
                print("Rest note!\n");
                lguSleep(durationInSeconds);
                continue;
            }

            const float dutyCycle = 15.0f;// 15% - This controls the perceived loudness, but too much will distort the sound
            const float pwmOffset = 0.0f;

            // Calculate how many cycles based on tone.duration (in ms)
            const float frequency = tone.note * factor;
            const float period = 1.0f / frequency;
            const float pwmCycles = durationInSeconds / period;

            if (lgTxPwm(handle, GPIO_PIN, frequency, dutyCycle, pwmOffset, pwmCycles) < 0) {
                print_error("Failed to transmit PWM\n");
            } else {
                print("Transmitted PWM!\n");
                while(lgTxBusy(handle, GPIO_PIN, LG_TX_PWM)) {
                    lguSleep(0.01);
                }
                print("PWM transmission complete!\n");
            }
        }
    }

	if (lgGpiochipClose(handle) != LG_OKAY) {
        free(filename);
        free(melody->tones);
        free(melody);
		print_error("Failed to close GPIO chip\n");
		return 1;
    }

    print("Closed GPIO chip!\n");

    free(filename);
    free(melody->tones);
    free(melody);

    return 0;
}