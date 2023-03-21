# file-compat tests

## Linux, macOS, Windows

```
mkdir -p build && cd build
cmake .. && cmake --build . && ctest -C Debug --verbose
```

## Emscripten

```
mkdir -p build && cd build
emcmake cmake .. && cmake --build . && emrun file_compat_test_c.html
```

## Android (build only)

```
mkdir -p build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$ANDROID_HOME/ndk/23.2.8568313/build/cmake/android.toolchain.cmake && cmake --build .
```
