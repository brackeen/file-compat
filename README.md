# file-compat

A C header file that redefines common `stdio` functions so that they work as expected on Windows and Android.

Additionally, includes the function `fc_resdir()` to get the path to the current executable's directory or its resources directory. This function works on Windows, Linux, macOS, iOS, and Android. On macOS and iOS, this is the path to the bundle's resources. On Windows and Linux, this is a path to the executable's directory. On Android and Emscripten, this is an empty string.

| Function            | Windows                      | Android
|---------------------|------------------------------|-----------------------------------------
| `printf`            | Uses `OutputDebugString`*    | Uses `__android_log_print`
| `fopen`             | Uses `fopen_s`               | Uses `AAssetManager_open` if read mode
| `fclose`            | Adds `NULL` check            | No change
| `chdir`             | Uses `_chdir`                | No change
| `sleep` / `usleep`  | Uses `Sleep`                 | No change

**`OutputDebugString` is only used if the debugger is present and no console is allocated. Otherwise uses `printf`.*

## Usage
The goal of `file_compat.h` is to allow a developer to write code using common `stdio` functions that will work across platforms. To use, include the header and use `fopen`, `printf`, etc. like normal.

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
