## Налаштування складових імені встановлюваної бібліотеки

Імʼя бібліотеки, яке бачать проекти-споживачі (бінарник бібліотеки, субдиректорія
`include/<name>/` для публічних заголовків, директорія CMake package і простір
імен імпортованої цілі що експортується через `install(EXPORT ...)`) виводиться
з трьох опціональних CMake-опцій. Вони дозволяють паралельним встановленням
різних версій бібліотеки співіснувати у системі - наприклад
`include/ImagesAnnotatorDataExporters-0.15.1-dev/` поруч з
`include/ImagesAnnotatorDataExporters-0.15/`.

| Опція | За замовчуванням | Ефект |
|---|---|---|
| `-DIADE_LIB_INCLUDE_MINOR_IN_NAME=OFF` | `ON` | Додає `.<minor>` до імені бібліотеки |
| `-DIADE_LIB_INCLUDE_MICRO_IN_NAME=ON` | `OFF` | Додає `.<micro>` (вмикає прапорець minor неявно) |
| `-DIADE_LIB_NAME_SUFFIX=-dev` | `""` | Додає довільний завершальний суфікс |

Складова minor включена за замовчуванням, оскільки простір імен публічного API
несе у собі пару "мажорна і мінорна версія" (`ImagesAnnotatorDataExporters015`),
а також тому, що [залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md)
іменує себе точно так само. Тому два мінорні випуски встановлюються цілком
паралельно - їхні бінарники, директорії заголовків і CMake-пакунки відрізняються.

Приклади імен бібліотеки для проекту `0.15.1`:

| Прапорці конфігурації | Імʼя бібліотеки | Згенерований бінарник |
|---|---|---|
| (немає) | `ImagesAnnotatorDataExporters-0.15` | `libImagesAnnotatorDataExporters-0.15.so` |
| `-DIADE_LIB_INCLUDE_MINOR_IN_NAME=OFF` | `ImagesAnnotatorDataExporters-0` | `libImagesAnnotatorDataExporters-0.so` |
| `-DIADE_LIB_INCLUDE_MICRO_IN_NAME=ON` | `ImagesAnnotatorDataExporters-0.15.1` | `libImagesAnnotatorDataExporters-0.15.1.so` |
| `-DIADE_LIB_NAME_SUFFIX=-dev` | `ImagesAnnotatorDataExporters-0.15-dev` | `libImagesAnnotatorDataExporters-0.15-dev.so` |
| `-DIADE_LIB_INCLUDE_MICRO_IN_NAME=ON -DIADE_LIB_NAME_SUFFIX=-dev` | `ImagesAnnotatorDataExporters-0.15.1-dev` | `libImagesAnnotatorDataExporters-0.15.1-dev.so` |

Приклад поєднаної конфігурації:

```
# всередині кореневої директорії проекту

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/шлях/до/префіксу/встановлення/драйверів/даних \
  -DIADE_LIB_INCLUDE_MICRO_IN_NAME=ON \
  -DIADE_LIB_NAME_SUFFIX=-dev
cmake --build build -j$(nproc)
cmake --install build --prefix /usr/local
```

Однакове імʼя послідовно використовується для кожного встановлюваного артефакту.
За замовчуванням це дає:

- `<prefix>/lib/libImagesAnnotatorDataExporters-0.15.so.0.15.1` разом із
  символьним посиланням soname `libImagesAnnotatorDataExporters-0.15.so.0`
  (`SOVERSION` залишається мажорною версією) і `.so`-посиланням для розробки,
- `<prefix>/include/ImagesAnnotatorDataExporters-0.15/*.h` - публічні заголовки з
  [src/lib/facade/public](/src/lib/facade/public), оголошені через
  `INSTALL_INTERFACE` цілі бібліотеки,
- `<libdir>/cmake/ImagesAnnotatorDataExporters-0.15/` зі згенерованими файлами
  `Config.cmake`, `ConfigVersion.cmake` і `Targets.cmake`, а також імпортовану
  ціль `ImagesAnnotatorDataExporters-0.15::ImagesAnnotatorDataExporters-0.15`.

Тому проект-споживач пише `find_package(ImagesAnnotatorDataExporters-0.15 0.15
REQUIRED)` для типового встановлення і має вживати змінене імʼя дослівно, якщо
складові були налаштовані інакше. Простір імен C++ у сирцевих файлах,
`ImagesAnnotatorDataExporters015`, цими опціями не змінюється.

Виведення імʼя реалізоване у
[cmake/imagesannotator-dataexporters-misc-variables-declare.cmake](/cmake/imagesannotator-dataexporters-misc-variables-declare.cmake)
і доступне як CMake cache-змінна `IADE_LIBRARY_NAME`, яку використовує решта
системи побудови, включно з іменем директорії HTML-виводу Doxygen. Вмикання
`IADE_LIB_INCLUDE_MICRO_IN_NAME` без `IADE_LIB_INCLUDE_MINOR_IN_NAME` спричиняє
попередження на стадії конфігурування, а прапорець minor вмикається неявно щоб
згенероване імʼя залишалось синтаксично коректним.

Імʼя [залежності від драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md)
не залежить від цих опцій: воно обирається окремою cache-змінною
`IADE_DATA_DRIVERS_PACKAGE`.
