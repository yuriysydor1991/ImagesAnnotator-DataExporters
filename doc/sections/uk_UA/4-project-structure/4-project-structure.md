# Структура проекту

Репозиторій будує єдину спільну бібліотеку - `libImagesAnnotatorDataExporters-0.11.so`, - яка перетворює базу даних анотацій ImagesAnnotator на набір даних для навчання на диску. Виконуваного файлу немає: окрім самої бібліотеки збірка створює лише тестові бінарники.

Експортери було винесено із застосунку [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) (з його компонента `src/annotator-business/exporters`), щоб застосунок і будь-який інший інструмент могли створювати ті самі набори даних без дублювання коду. Самі записи анотацій тут не визначено: їх читають через бібліотеку [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), яка раніше забрала базу даних анотацій із того самого застосунку.

Розкладка верхнього рівня:

| Шлях | Вміст |
|---|---|
| [src/lib/facade/public](/src/lib/facade/public) | встановлювані публічні заголовки - увесь інтерфейс `ImagesAnnotatorDataExporters011` |
| [src/lib/facade/LibraryFacade.cpp](/src/lib/facade/LibraryFacade.cpp) | реалізація фасаду, точка входу бібліотеки |
| [src/lib/libmain](/src/lib/libmain) | `LibMain.cpp` та `LibFactory.cpp` - ядро реалізації |
| [src/lib/cmake](/src/lib/cmake) | генерація встановлюваного CMake-пакунка бібліотеки |
| [src/exporters](/src/exporters) | по одній піддиректорії на родину розкладок наборів даних - `PlainTxt`, `Yolo4`, `Ultralytics`, `Coco` та `PyTorch`, - кожна несе власні реалізації `IExporter`, свій `CMakeLists.txt` і свої модульні тести, плюс спільний для всіх `ExportersAliases.h` |
| [src/croppers](/src/croppers) | `create_builtin_cropper()` і необовʼязковий OpenCV-`IImageCropperFacility` за ним |
| [src/helpers](/src/helpers) | `IHelper.h`, `TypeHelper.h`, `ImageRecordUrlAndPathHelper`, `ImageLoader` |
| [src/CURL](/src/CURL) | `CURLController` - обгортка libcurl для попереднього звантаження вебзображень |
| [src/log](/src/log) | макроси журналювання і простий логер, вкомпільований у бібліотеку |
| [cmake](/cmake) | система збірки: опції компіляції та вмикачі залежностей |
| [doc](/doc) | ця документація і діаграми проекту |
| [misc](/misc) | конвеєр Jenkins і файли Docker |

Реалізація живе у просторах імен `iade0impl`, `iannotator::exporters`, `iannotator::exporters::helpers` та `iannotator::exporters::curli`. Жоден із них не встановлюється - проект-споживач бачить лише `ImagesAnnotatorDataExporters011` (рекомендовано аліас `iade`).

1. [Діаграми проекту](/doc/sections/uk_UA/4-project-structure/4-0-project-diagrams.md)
1. [Де живе реалізація експортерів](/doc/sections/uk_UA/4-project-structure/4-1-implement-code-straight-away.md)
1. [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md)
1. [Розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md)
1. [Публічні інтерфейсні файли бібліотеки](/doc/sections/uk_UA/4-project-structure/4-8-the-librarys-installable-include-header-files.md)
1. [Версіювання і інші параметри проекту](/doc/sections/uk_UA/4-project-structure/4-3-version-tracking-and-other-project-parameters.md)
1. [Тести проекту](/doc/sections/uk_UA/4-project-structure/4-4-project-tests.md)
    1. [Фреймворк тестів Google Test](/doc/sections/uk_UA/4-project-structure/4-4-1-google-test.md)
