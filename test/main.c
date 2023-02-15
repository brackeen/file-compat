/*
Windows:
cl /Wall /I .. main.c test_write.cpp

Linux:
gcc -Wall -Wpedantic -I .. main.c test_write.cpp && ./a.out

macOS:
clang -framework Foundation -Weverything -I .. main.c test_write.cpp && ./a.out

Emscripten:
emcc -lidbfs.js -I .. main.c test_write.cpp -o main.html && emrun main.html

*/

#if defined(__ANDROID__)
void *android_activity;
#define FILE_COMPAT_ANDROID_ACTIVITY android_activity
#endif

#include "file_compat.h"
#include "test_write.h"

static void test_basic(void) {
    char locale[128];
    fc_locale(locale, sizeof(locale));
    printf("Locale (full):  \"%s\"\n", locale);

    fc_locale(locale, 3);
    printf("Locale (short): \"%s\"\n", locale);

    char path[PATH_MAX];
    fc_resdir(path, PATH_MAX);
    printf("Resources dir: \"%s\"\n", path);
}

int main(void) {
    test_basic();
    test_file_writing();
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    android_activity = activity;
    main();
}
#endif
