## Публічні інтерфейсні файли бібліотеки

Директорія [src/lib/facade/public](/src/lib/facade/public) містить увесь встановлюваний інтерфейс бібліотеки - п'ятнадцять заголовків, усі у просторі імен `ImagesAnnotatorDataExporters011`. Шість із них є власне API і лежать у корені директорії:

| Заголовок | Оголошує |
|---|---|
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | статичні методи `LibraryFacade`, точку входу бібліотеки |
| [ILib.h](/src/lib/facade/public/ILib.h) | `ILib::perform_export()` - виконує експорт, описаний `LibraryContext` |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | методи доступу до даних `perform_export()` та `export_db()`, разом із вихідним `get_exporter()` |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | `IExporter::export_db()` - окремий експортер, використаний самостійно |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | інтерфейс, який реалізує проект-споживач, щоб вирізати зображення |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | макрос видимості `IADE_API` |

Решта дев'ять є нащадками `LibraryContext`, специфічними для розкладок - по одному на кожну розкладку набору даних, яку пише бібліотека, - і лежать разом у піддиректорії [contexts](/src/lib/facade/public/contexts), а не поруч із шістьма вище, бо їх більше, ніж усього іншого інтерфейсу разом узятого, і кожен відрізняється від сусідів лише розкладкою, яку називає:

| Заголовок | Оголошує |
|---|---|
| [contexts/PlainTxtExportLibraryContext.h](/src/lib/facade/public/contexts/PlainTxtExportLibraryContext.h) | `LibraryContext` розкладки простого тексту |
| [contexts/Yolo4ExportLibraryContext.h](/src/lib/facade/public/contexts/Yolo4ExportLibraryContext.h) | `LibraryContext` розкладки YOLO v4 (darknet) |
| [contexts/UltralyticsDetectExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsDetectExportLibraryContext.h) | `LibraryContext` розкладки детекції Ultralytics YOLO |
| [contexts/UltralyticsObbExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsObbExportLibraryContext.h) | `LibraryContext` розкладки орієнтованих обмежувальних рамок Ultralytics YOLO |
| [contexts/UltralyticsSegmentExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsSegmentExportLibraryContext.h) | `LibraryContext` розкладки сегментації Ultralytics YOLO |
| [contexts/CocoExportLibraryContext.h](/src/lib/facade/public/contexts/CocoExportLibraryContext.h) | `LibraryContext` розкладки детекції об'єктів COCO |
| [contexts/PascalVocExportLibraryContext.h](/src/lib/facade/public/contexts/PascalVocExportLibraryContext.h) | `LibraryContext` розкладки набору даних Pascal VOC |
| [contexts/CreateMLExportLibraryContext.h](/src/lib/facade/public/contexts/CreateMLExportLibraryContext.h) | `LibraryContext` розкладки набору даних для детекції об'єктів Create ML |
| [contexts/PyTorchExportLibraryContext.h](/src/lib/facade/public/contexts/PyTorchExportLibraryContext.h) | `LibraryContext` розкладки PyTorch Vision разом із її обрізачем зображень |

Кожен із тих дев'яти дістається шістьох вище через підключення з `../`, і саме це лишає їх розв'язними після встановлення: коренем підключення споживача є директорія, яка містить `ImagesAnnotatorDataExporters-0.11/`, тож звичайне `#include "LibraryContext.h"` зсередини `contexts/` шукало б його у тому корені й не знайшло б. Споживача це не стосується: він дістається всіх дев'яти через `LibraryFacade.h`, який їх підключає, і лише проект, що виписує заголовок розкладки напряму, називає складову `contexts/` сам.

[src/lib/facade/CMakeLists.txt](/src/lib/facade/CMakeLists.txt) встановлює директорію цілком під `include/${PROJECT_LIBRARY_NAME}`, що для поточної назви і версії дає `include/ImagesAnnotatorDataExporters-0.11/`. І ту піддиректорію, і звичайний корінь підключення експортує ціль бібліотеки, тож споживач може писати будь-яку з двох форм:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>  // рекомендована
#include <LibraryFacade.h>                                 // теж працює
```

Форма з префіксом є безпечною - імена на кшталт `IExporter.h` достатньо загальні, щоб зіткнутися у насиченому шляху підключення.

### Чому видно лише ці заголовки

Бібліотека будується з `CXX_VISIBILITY_PRESET hidden`, тож спільний обʼєкт залишають лише сутності, позначені `IADE_API`. Це вимога коректності, а не оптимізація розміру. Бібліотека ImagesAnnotatorDataDrivers, з якою ця лінкується, походить із того самого шаблону проекту і експортує власні символи `default_logger::DefaultLogger` та `project_decls`. Якби обидва набори експортувалися, динамічний компонувальник звʼязав би виклики однієї бібліотеки з визначеннями іншої.

Сама лише видимість лишає одну дірку. Інстанціація `std::make_shared` називає свій клас у власному спотвореному імені й лишається слабкою та експортованою, якою б не була видимість, тож простір імен реалізації тут - `iade0impl`, а не `lib0impl`, який використовує шаблон проекту, а разом із ним і бібліотека драйверів даних. Два класи `LibFactory` не поділяють навіть розкладки vtable, і до перейменування компонувальник справді звʼязував `std::make_shared<lib0impl::LibFactory>()` однієї бібліотеки з визначенням іншої.

Отже, новий публічний клас належить до [src/lib/facade/public](/src/lib/facade/public) - новий контекст розкладки набору даних до її піддиректорії [contexts](/src/lib/facade/public/contexts), звідки він дістається заголовків над собою через `../`, - і має бути позначений `IADE_API`; кожен інший компонент під [src](/src) лишається приватним для спільного обʼєкта, і дістатися до нього можна лише через абстрактні інтерфейси вище.

### Встановлюваний CMake-пакунок

Разом із бінарником і заголовками збірка встановлює CMake-пакунок, згенерований [src/lib/cmake/lib-cmake-module-gen.cmake](/src/lib/cmake/lib-cmake-module-gen.cmake) із [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in). Він потрапляє до `<libdir>/cmake/ImagesAnnotatorDataExporters-0.11/` і складається з трьох файлів: експортовані цілі, `ImagesAnnotatorDataExporters-0.11ConfigVersion.cmake`, записаний `write_basic_package_version_file()` із сумісністю `SameMajorVersion`, та `ImagesAnnotatorDataExporters-0.11Config.cmake`, який виконує `find_dependency()` пакунка драйверів даних перед підключенням цілей - публічні заголовки називають його типи записів, тож його треба розвʼязати першим.

Тому проекту нижче за течією не потрібно нічого більшого за:

```cmake
find_package(ImagesAnnotatorDataExporters-0.11 0.11 REQUIRED)

target_link_libraries(
  your_target
  PRIVATE ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11
)
```

Див. [Встановлення](/doc/sections/uk_UA/7-installing/7-installing.md) та [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).
