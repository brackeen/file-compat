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
int test_writeable_dir(const char *dir_name, const char *datadir_path) {
    char path[PATH_MAX];
    strcpy(path, datadir_path);
    strcat(path, "count.txt");

    printf("%s dir: \"%s\"\n", dir_name, datadir_path);

    // Read existing count
    int count = 0;
    FILE *file = fopen(path, "r");
    if (file) {
        if (fscanf(file, "%i", &count) != 1) {
            printf("Error: Couldn't read existing file\n");
            return -1;
        }
        fclose(file);
    }

    // Write new count
    count++;
    file = fopen(path, "w");
    if (!file) {
        printf("Error: Couldn't create file\n");
        return -1;
    }
    fprintf(file, "%i\n", count);
    fclose(file);

    // Check if new count was written
    int countCheck = 0;
    file = fopen(path, "r");
    if (!file) {
        printf("Error: Couldn't open newly created file\n");
        return -1;
    }
    if (fscanf(file, "%i", &countCheck) != 1) {
        printf("Error: Couldn't read newly created file\n");
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

int main(void) {
    test_basic();

    static char data_path[PATH_MAX];
    static char cache_path[PATH_MAX];
    if (fc_datadir("FileCompatTestApp", data_path, PATH_MAX)) {
        printf("Error: Couldn't get data dir\n");
        return -1;
    }
    if (fc_cachedir("FileCompatTestApp", cache_path, PATH_MAX)) {
        printf("Error: Couldn't get cache dir\n");
        return -1;
    }

#if !defined(__EMSCRIPTEN__)
    int result = 0;
    result += test_writeable_dir("Data", data_path);
    result += test_writeable_dir("Cache", cache_path);
    return result;
#else
    // Load contents of the path into memory
    EM_ASM({
        let data_path = UTF8ToString($1);
        let cache_path = UTF8ToString($3);
        FS.mount(IDBFS, {}, data_path);
        FS.mount(IDBFS, {}, cache_path);
        FS.syncfs(true, function (err) {
            assert(!err);
            // Test writing
            _test_writeable_dir($0, $1);
            _test_writeable_dir($2, $3);
            // Persist contents of files
            FS.syncfs(function (err) {
                assert(!err);
            });
        });
    }, "Data", data_path, "Cache", cache_path);
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
