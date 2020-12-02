/*
Windows:
cl /Wall /I .. main.c

Linux:
clang -Weverything -I .. main.c && ./a.out

macOS:
clang -framework Foundation -Weverything -I .. main.c && ./a.out

Emscripten:
emcc -lidbfs.js -I .. main.c -o main.html && emrun main.html

*/
#if defined(__ANDROID__)
static void *androidActivity;
#define FILE_COMPAT_ANDROID_ACTIVITY androidActivity
#endif

#include "file_compat.h"

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

#if defined(__EMSCRIPTEN__)
EMSCRIPTEN_KEEPALIVE
#ifdef __cplusplus
     extern "C"
#endif
#else
static
#endif
int test_datadir(const char *datadir_path) {
    char path[PATH_MAX];
    strcpy(path, datadir_path);
    strcat(path, "count.txt");

    printf("Data dir: \"%s\"\n", datadir_path);

    // Read existing count
    int count = 0;
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

#if defined(__EMSCRIPTEN__)
    // Persist contents of file
    EM_ASM(
        FS.syncfs(function (err) {
            assert(!err);
        });
    );
#endif

    return 0;
}

int main(void) {
    test_basic();

    static char path[PATH_MAX];
    if (fc_datadir("FileCompatTestApp", path, PATH_MAX)) {
        printf("Error: Couldn't get data dir\n");
        return -1;
    }

#if !defined(__EMSCRIPTEN__)
    return test_datadir(path);
#else
    // Load contents of the path into memory
    EM_ASM({
        var path = UTF8ToString($0);
        FS.mount(IDBFS, {}, path);
        FS.syncfs(true, function (err) {
            assert(!err);
            _test_datadir($0);
        });
    }, path);
    return 0;
#endif
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    androidActivity = activity;
    main();
}
#endif
