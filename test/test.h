#include <assert.h>
#include "file_compat.h"

#if defined(__ANDROID__)
static ANativeActivity *android_activity;
#define FILE_COMPAT_ANDROID_ACTIVITY android_activity
#endif

#if defined(_MSC_VER)
#define strcpy(dst, src) strcpy_s((dst), sizeof(dst), (src))
#define strcat(dst, src) strcat_s((dst), sizeof(dst), (src))
#define fscanf fscanf_s
#endif

static const char APP_ID[] = "FileCompatTestApp";

static int test_writeable_dir(const char *dir_name, const char *datadir_path) {
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

static int test_file_writing(void) {
    char data_path[PATH_MAX];
    char cache_path[PATH_MAX];
    if (fc_datadir(APP_ID, data_path, PATH_MAX)) {
        printf("Error: Couldn't get data dir\n");
        return -1;
    }
    if (fc_cachedir(APP_ID, cache_path, PATH_MAX)) {
        printf("Error: Couldn't get cache dir\n");
        return -1;
    }

    int result = 0;
    result += test_writeable_dir("Data", data_path);
    result += test_writeable_dir("Cache", cache_path);
    return result;
}

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

static void print_lang(void) {
#if defined(__OBJC__)
#  if __has_feature(objc_arc)
    const char *arc = "ARC";
#  else
    const char *arc = "NO ARC";
#  endif
#  if defined(__cplusplus)
    printf("Objective-C++ (%s)\n", arc);
#  else
    printf("Objective-C (%s)\n", arc);
#  endif
#endif
#if defined(__cplusplus)
    printf("__cplusplus=%li\n", __cplusplus);
#endif
#if defined(__STDC_VERSION__)
    printf("__STDC_VERSION__=%li\n", __STDC_VERSION__);
#endif
}

#if defined(__EMSCRIPTEN__)
#  ifdef __cplusplus
extern "C"
#  endif
EMSCRIPTEN_KEEPALIVE int test_all(void);
#else
static
#endif
int test_all(void) {
    test_basic();
    return test_file_writing();
}

#if defined(__EMSCRIPTEN__)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"

EM_JS(void, sync_and_test_all, (const char *data_path, const char *cache_path), {
    // Load contents of the paths into memory
    FS.mount(IDBFS, {}, UTF8ToString(data_path));
    FS.mount(IDBFS, {}, UTF8ToString(cache_path));
    FS.syncfs(true, function (err) {
        assert(!err);
        var result = _test_all();
        // Persist contents of files
        FS.syncfs(function (err) {
            assert(!err);
            console.log("Result: " + result);
        });
    });
})

#pragma clang diagnostic pop

#endif

int main(void) {
    print_lang();

#if defined(__EMSCRIPTEN__)
    char data_path[PATH_MAX];
    char cache_path[PATH_MAX];
    fc_datadir(APP_ID, data_path, PATH_MAX);
    fc_cachedir(APP_ID, cache_path, PATH_MAX);
    sync_and_test_all(data_path, cache_path);
    return 0;
#else
    return test_all();
#endif
}

#if defined(__ANDROID__)
JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity *activity, void *saved_state, size_t saved_state_size) {
    (void)saved_state;
    (void)saved_state_size;
    android_activity = activity;
    print_lang();
    int result = test_all();
    printf("Result: %i\n", result);
}
#endif
