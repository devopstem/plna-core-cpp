# Contributing

## Build (portable core — Linux/CI/Windows toolchain)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Android JNI собирается только с NDK (`ANDROID` / `PLNA_BUILD_ANDROID_JNI`).

## Test
```bash
ctest --test-dir build --output-on-failure
```
(тестов пока мало / нет — добавляйте рядом с `plna_core`)

## Code Style
- C++23, `-Wall -Wextra -Werror` в CI
- Геометрия и draw list — только в `core/`; платформенный glue — в `platform/*`
- Схема Firebase → сцена: `docs/DATA_MODEL.md`