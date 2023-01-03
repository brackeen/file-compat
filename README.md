# file-compat

A C header file to help write cross-platform code related to `stdio` functions.

## Redefined `stdio` functions
These functions are redefined so that they work as expected on Windows and Android.

| Function            | Windows                      | Android
|---------------------|------------------------------|----------------------------------------------
| `printf`            | Uses `OutputDebugString`     | Uses `__android_log_print`
| `fopen`             | Uses `fopen_s`               | Uses `AAssetManager_open` if read mode
| `fclose`            | Adds `NULL` check            | No change

*Note: `OutputDebugString` is only used if the debugger is present and no console is allocated. Otherwise uses `printf`.*


## Added functions
These added functions work on Windows, Linux, macOS, iOS, Android, and Emscripten.

| Function     | Description
|--------------|-----------------------------------------------------------------------------------
| `fc_locale`  | Gets the user's preferred language (For example, "en-US")
| `fc_resdir`  | Gets the current executable's directory (Windows, Linux) or its resources directory (macOS, iOS)
| `fc_datadir` | Gets the current executable's data directory, useful for saving preferences.

## Usage
To use, include `file_compat.h` and use `fopen`, `printf`, etc. like normal.

```C
#include "file_compat.h"

...

char path[PATH_MAX];
fc_resdir(path, PATH_MAX);
strcat(path, "myfile.txt");
FILE *file = fopen(path, "r");

...

fclose(file);
```

For Android, define `FILE_COMPAT_ANDROID_ACTIVITY` to be a reference to an `ANativeActivity` instance (or to a function that returns an `ANativeActivity` instance). May be `NULL`, which means `fopen` won't open files in the APK.

```C
#define FILE_COMPAT_ANDROID_ACTIVITY app->activity
#include "file_compat.h"
```
