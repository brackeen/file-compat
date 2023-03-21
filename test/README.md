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
