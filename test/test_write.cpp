#ifndef __cplusplus
#error Compile this file as C++ to test if file_compat.h compiles as C++
#endif

#include <assert.h>

#include "file_compat.h"
#include "test_write.h"

#if defined(__ANDROID__)
extern ANativeActivity *android_activity;
#define FILE_COMPAT_ANDROID_ACTIVITY android_activity
#endif

static const char APP_ID[] = "FileCompatTestApp";

#if defined(__EMSCRIPTEN__)
EMSCRIPTEN_KEEPALIVE extern "C"
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

int test_file_writing(void) {
#if defined(__ANDROID__)
    assert(android_activity != NULL);
#endif

    static char data_path[PATH_MAX];
    static char cache_path[PATH_MAX];
    if (fc_datadir(APP_ID, data_path, PATH_MAX)) {
        printf("Error: Couldn't get data dir\n");
        return -1;
    }
    if (fc_cachedir(APP_ID, cache_path, PATH_MAX)) {
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

