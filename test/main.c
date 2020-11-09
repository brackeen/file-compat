/*
Windows:
cl /Wall /I .. main.c

Linux:
clang -Weverything -I .. main.c && ./a.out

macOS:
clang -framework Foundation -Weverything -I .. main.c && ./a.out

Emscripten:
emcc -Weverything -I .. main.c -o main.html && emrun main.html

*/
#if defined(__ANDROID__)
static void *androidActivity;
#define FILE_COMPAT_ANDROID_ACTIVITY androidActivity
#endif

#include "file_compat.h"

int main(void) {
    char locale[128];
    fc_locale(locale, sizeof(locale));
    printf("Locale (full):  \"%s\"\n", locale);

    fc_locale(locale, 3);
    printf("Locale (short): \"%s\"\n", locale);

    char path[PATH_MAX];
    fc_resdir(path, PATH_MAX);
    printf("Resources dir: \"%s\"\n", path);

    if (fc_datadir("FileCompatTestApp", path, PATH_MAX)) {
        printf("Error: Couldn't get data dir\n");
        return -1;
    }
    printf("Data dir: \"%s\"\n", path);

    // Read existing count
    int count = 0;
    strcat(path, "count.txt");
    FILE *file = fopen(path, "r");
    if (file) {
        if (fscanf(file, "%i", &count) != 1) {
            printf("Error: Couldn't read existing file from data dir\n");
            return -1;
        }
        fclose(file);
    }

    // Write new count
    count++;
    file = fopen(path, "w");
    if (!file) {
        printf("Error: Couldn't create file in data dir\n");
        return -1;
    }
    fprintf(file, "%i\n", count);
    fclose(file);

    // Check if new count was written
    int countCheck = 0;
    file = fopen(path, "r");
    if (!file) {
        printf("Error: Couldn't open newly created file in data dir\n");
        return -1;
    }
    if (fscanf(file, "%i", &countCheck) != 1) {
        printf("Error: Couldn't read newly created file from data dir\n");
        return -1;
    }
    fclose(file);
    if (count == countCheck) {
        printf("The file count.txt was written to %i time(s)\n", count);
    } else {
        printf("Error: invalid count (was %i, should be %i)\n", countCheck, count);
        return -1;
    }

    return 0;
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    androidActivity = activity;
    main();
}
#endif
