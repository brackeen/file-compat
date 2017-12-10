/*
Windows:
cl /Wall /I .. main.c

Windows XP:
cl /D_USING_V110_SDK71_ /I .. main.c /link /SUBSYSTEM:CONSOLE,5.01

Linux:
clang -Weverything -I .. main.c && ./a.out

macOS:
clang -framework Foundation -Weverything -I .. main.c && ./a.out

Emscripten:
$EMSCRIPTEN_ROOT_PATH/emcc -Weverything -I .. main.c -o main.html && open main.html

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
    printf("Resources path: \"%s\"\n", path);

    printf("Sleeping 2 seconds...\n");
    sleep(2);
    printf("Done\n");

    return 0;
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    androidActivity = activity;
    main();
}
#endif
