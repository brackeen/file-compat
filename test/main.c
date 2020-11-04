/*
Windows:
cl /Wall /I .. main.c

Windows XP:
cl /D_USING_V110_SDK71_ /I .. main.c /link /SUBSYSTEM:CONSOLE,5.01

Linux:
clang -Weverything -I .. main.c && ./a.out

macOS:
clang -framework Foundation -framework Security -Weverything -I .. main.c && ./a.out

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

#if defined(__APPLE__) || defined(__ANDROID__)
    fc_datadir("com.brackeen.file_compat_test", path, PATH_MAX);
    printf("Data dir: \"%s\"\n", path);

    strcat(path, "test.txt");

    FILE *file = fopen(path, "w");
    if (file) {
        fprintf(file, "This text was written to the data dir");
        fclose(file);
    } else {
        printf("Writing failed\n");
    }

    file = fopen(path, "r");
    if (file) {
        char buffer[80];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s\n", buffer);
        }
        fclose(file);
    } else {
        printf("Reading failed\n");
    }
#endif

    return 0;
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    androidActivity = activity;
    main();
}
#endif
