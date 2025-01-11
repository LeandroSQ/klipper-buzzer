#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
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

int main(int argc, char **argv) {
    // Arguments:
    // <filename> or none
    if (argc > 2) {
        print("Usage: %s [filename]\n", argv[0]);
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
        filename = (char*)getRandomFileInFolder(isDirectory ? filename : SONGS_FOLDER);
        if (filename == NULL) {
            print("Failed to get random file in folder %s\n", isDirectory ? filename : SONGS_FOLDER);
            return 1;
        }

        print("Selected random file: %s\n", filename);
    } else {
        print("Selected file: %s\n", filename);
    }

    // Parse the melody
    Melody* melody = parseMelodyFile(filename);
    if (melody == NULL) {
        print("Failed to parse melody\n");
        free(filename);
        return 1;
    }

	int handle = lgGpiochipOpen(GPIO_CHIP);
	if (handle != LG_OKAY) {
        free(filename);
        free(melody->tones);
        free(melody);
		print("Failed to open GPIO chip\n");
		return 1;
	} else {
        print("Opened GPIO chip!\n");
    }

    if (lgGpioClaimOutput(handle, 0, GPIO_PIN, 0) != LG_OKAY) {
        print("Failed to claim GPIO\n");
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
                print("Failed to transmit PWM\n");
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
		print("Failed to close GPIO chip\n");
		return 1;
    }

    print("Closed GPIO chip!\n");

    free(filename);
    free(melody->tones);
    free(melody);

    return 0;
}