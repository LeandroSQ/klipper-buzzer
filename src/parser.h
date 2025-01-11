#ifndef _PARSER_H_
#define _PARSER_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include "util.h"

#ifndef MAX_FILES
    #define MAX_FILES 1024
#endif

#ifndef MAX_TONES
    #define MAX_TONES 2048
#endif

typedef struct {
    int note;
    int duration;
} Tone;

typedef struct {
    int pitch;
    Tone* tones;
    int length;
} Melody;

/**
 * @brief Get the Random File In Folder
 * The caller is responsible for freeing the returned string
 *
 * @param path The path to the folder
 * @return const char* The random file in the folder
 */
const char* getRandomFileInFolder(const char* path);

/**
 * @brief Given a filename, parse the melody file and return a Melody object
 * The caller is responsible for freeing the returned Melody object
 *
 * @param path The path to the melody file
 * @return Melody* The parsed melody
 */
Melody* parseMelodyFile(const char* path);

#endif