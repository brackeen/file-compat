# file-compat tests

## Automated tests

The [test.yml](../.github/workflows/test.yml) GitHub Action uses the [CMakeLists.txt](CMakeLists.txt) file in this directory to test `file_compat.h` for the following platforms and languages:

* Linux: C, C++
* Windows: C, C++
* macOS: C, C++, Objective-C, Objective-C++ (both with and without ARC).

macOS executables are automatically tested, but bundled sandboxed apps are not.

## Testing manually

Additionally, these can be built and tested manually:
* Android: C, C++
* Emscripten: C, C++

The iOS version does not have tests, but it uses the same code as macOS.

### Linux, macOS, Windows
```
cmake -B build && cmake --build build && ctest --test-dir build -C Debug --verbose
```

### Emscripten

```
emcmake cmake -B build && cmake --build build && emrun build/file_compat_test_c.html
```

### Android (build only)

```
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$ANDROID_HOME/ndk/23.2.8568313/build/cmake/android.toolchain.cmake && cmake --build build
```
