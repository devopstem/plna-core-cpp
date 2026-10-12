# PLNA Engine — C++23 Core

Единое нативное ядро чертежа торгового зала для **Android · iOS · Windows**: один алгоритм карты/маршрута, тонкий UI на каждой платформе (без тяжёлой геометрии в Kotlin → меньше CPU и батареи).

Данные чертежа приходят из Firebase RTDB `store_maps` (стеллажи, холодильники, стены, маркеры) — хост парсит JSON **один раз** и заливает в C++.

| | |
|---|---|
| Язык | C++23 |
| Сборка | CMake → `plna_core` (+ `libplna.so` с JNI на Android) |
| Лицензия | MIT |
| Статус | Hobby / experimental |

## Модули

| Модуль | Namespace | Роль |
|--------|-----------|------|
| Сцена карты | `plna::map` | Пол, racks/coolers/stands, стены, маркеры, draw list, маршрут |
| Геометрия / товары | `plna::math` | Полки, кэш штрихкодов, обёртка маршрута |
| Сеть | `plna::net` | TCP + framing (live-задачи; POSIX пока) |
| Android glue | `platform/android` | JNI → `NativeMapEngine` / `NativeNetwork` |

```
Firebase store_maps  →  Host FFI (раз в sync)  →  plna_core
                                                      ↓
                         Canvas / Metal / DXGI  ←  buildDrawList()
```

## Документация

- **[docs/C_API.md](docs/C_API.md)** — как вызывать с любой платформы (рекомендуемый путь)
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** — слои, батарея, JNI
- **[docs/DATA_MODEL.md](docs/DATA_MODEL.md)** — Firebase `store_maps` → сцена
- **[docs/ISSUES.md](docs/ISSUES.md)** — оставшийся долг

## Сборка

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure

# быстрый просмотр твоего JSON:
./build/plna_map_dump fixtures/store_map_sample.json --route 1 1 28 20
```

Android NDK: тот же CMake с `android.toolchain.cmake` → `libplna.so` (JNI + core).

## Контракт хоста (комфортный)

```text
Firebase JSON  →  plna_map_load_json(...)  один раз
                 →  plna_map_build_draw_list() только если revision изменился
                 →  Canvas / Metal / DXGI paint
```

Либо поштучно `plna_map_add_*`, либо сразу весь `store_maps`. На Android добавлены `nativeLoadStoreMapJson` / `nativeRevision`.

## Contributing

См. [CONTRIBUTING.md](CONTRIBUTING.md).
