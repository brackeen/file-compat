# file-compat tests

## Automated tests

The [test.yml](../.github/workflows/test.yml) GitHub Action uses the [CMakeLists.txt](CMakeLists.txt) file in this directory to test `file_compat.h` for the following platforms and languages:

* Linux: C, C++
* Windows: C, C++
* macOS: C, C++, Objective-C, Objective-C++ (both with and without ARC).
* Emscripten: C, C++ (build only, no test)
* Android: C, C++ (build only, no test)

The tests involve reading and writing to files in the directories from `fc_datadir` and `fc_cachedir`.

On macOS, plain executables are tested, but bundled sandboxed apps are not.

The iOS version does not have tests, but it uses the same code as macOS.

## Testing manually

### Linux
```
cmake -B build/linux && cmake --build build/linux && ctest --test-dir build/linux --verbose
```

### macOS
```
cmake -B build/macos && cmake --build build/macos && ctest --test-dir build/macos --verbose
```

### Windows
```
cmake -B build\windows && cmake --build build\windows && ctest --test-dir build\windows -C Debug --verbose
```

### Emscripten (build and run in browser)

```
emcmake cmake -B build/emscripten && cmake --build build/emscripten && emrun build/emscripten/file_compat_test_c.html
```

### Android (build only)

```
cmake -B build/android -DCMAKE_TOOLCHAIN_FILE=$ANDROID_HOME/ndk/23.2.8568313/build/cmake/android.toolchain.cmake && cmake --build build/android
```
