#include "parser.h"
#include <stdbool.h>
#include <string.h>
#include "vector.h"

const char* getRandomFileInFolder(const char* path) {
    // Get all .txt files in a given folder
    DIR *folder = opendir(path);
    if (folder == NULL) {
        print("Failed to open folder: %s\n", path);
        return NULL;
    }

    struct dirent *entry;
    char *files[MAX_FILES];
    int count = 0;

    while ((entry = readdir(folder))) {
        // Ignore hidden files
        if (entry->d_type != DT_REG) continue;

        // Ignore non .txt files
        const char* dot = strrchr(entry->d_name, '.');
        if (dot && strcmp(dot, ".txt") != 0) continue;

        if (count >= MAX_FILES) {
            print("Too many files in folder: %s\n", path);
            break;
        }

        files[count++] = strdup(entry->d_name);
    }

    closedir(folder);

    if (count == 0) {
        print("No files found in folder: %s\n", path);
        return NULL;
    }

    // Select a random file
    srand(time(NULL));
    int index = rand() % count;
    char* result = files[index];

    // Free all other files
    for (int i = 0; i < count; i++) {
        if (i != index) free(files[i]);
    }

    // Append the folder path
    char* fullPath = malloc(strlen(path) + strlen(result) + 2);
    strcpy(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, result);
    free(result);

    return fullPath;
}

Melody* parseMelodyFile(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        print("Failed to open file: %s\n", path);
        return NULL;
    }

    int pitch = 0;
    Vector *tones = createVector(sizeof(Tone));

    char* line = NULL;
    size_t length = 0;
    ssize_t read;
    bool isFirstLine = true;
    while ((read = getline(&line, &length, file)) != -1) {
        // Check if the file is %d or %d %d
        int a, b;
        if (isFirstLine) {
            if (sscanf(line, "%d", &a) == 1) {
                pitch = a;
                print("Parsed pitch: %d\n", pitch);
                isFirstLine = false;
            } else {
                print("Invalid first line: %s\n", line);
                freeVector(tones);
                free(line);
                fclose(file);
                return NULL;
            }
        } else if (sscanf(line, "%d %d", &a, &b) == 2) {
            print("Parsed tone: %d %d\n", a, b);
            Tone tone = { a, b };
            if (!vectorAppend(tones, &tone)) {  // Assuming vectorAppend returns success/failure
                print("Failed to append tone\n");
                free(line);
                freeVector(tones);
                fclose(file);
                return NULL;
            }
        } else {
            print("Invalid line: %s\n", line);
            freeVector(tones);
            free(line);
            fclose(file);
            return NULL;
        }
    }

    free(line);
    print("Parsed %d tones\n", tones->size);
    fclose(file);

    print("Copying data...");
    Melody* melody = malloc(sizeof(Melody));
    melody->pitch = pitch;
    melody->length = tones->size;
    melody->tones = malloc(sizeof(Tone) * tones->size);
    memcpy(melody->tones, tones->data, sizeof(Tone) * tones->size);
    freeVector(tones);

    return melody;
}