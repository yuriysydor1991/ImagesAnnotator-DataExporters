## Публічні інтерфейсні файли бібліотеки

Директорія [src/lib/facade/public](/src/lib/facade/public) містить увесь встановлюваний інтерфейс бібліотеки - девʼятнадцять заголовків, усі у просторі імен `ImagesAnnotatorDataExporters012`. Шість із них є власне API і лежать у корені директорії:

| Заголовок | Оголошує |
|---|---|
| [IADataExportersFacade.h](/src/lib/facade/public/IADataExportersFacade.h) | статичні методи `IADataExportersFacade`, точку входу бібліотеки |
| [IADataExportersLib.h](/src/lib/facade/public/IADataExportersLib.h) | `IADataExportersLib::perform_export()` - виконує експорт, описаний `IADataExportersContext` |
| [IADataExportersContext.h](/src/lib/facade/public/IADataExportersContext.h) | методи доступу до даних `perform_export()` та `export_db()`, разом із вихідним `get_exporter()` |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | `IExporter::export_db()` - окремий експортер, використаний самостійно |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | інтерфейс, який реалізує проект-споживач, щоб вирізати зображення |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | макрос видимості `IADE_API` |

Решта тринадцять є нащадками `IADataExportersContext`, специфічними для розкладок - по одному на кожну розкладку набору даних, яку пише бібліотека, - і лежать разом у піддиректорії [contexts](/src/lib/facade/public/contexts), а не поруч із шістьма вище, бо їх більше, ніж усього іншого інтерфейсу разом узятого, і кожен відрізняється від сусідів лише розкладкою, яку називає:

| Заголовок | Оголошує |
|---|---|
| [contexts/PlainTxtExportContext.h](/src/lib/facade/public/contexts/PlainTxtExportContext.h) | `IADataExportersContext` розкладки простого тексту |
| [contexts/Yolo4ExportContext.h](/src/lib/facade/public/contexts/Yolo4ExportContext.h) | `IADataExportersContext` розкладки YOLO v4 (darknet) |
| [contexts/UltralyticsDetectExportContext.h](/src/lib/facade/public/contexts/UltralyticsDetectExportContext.h) | `IADataExportersContext` розкладки детекції Ultralytics YOLO |
| [contexts/UltralyticsObbExportContext.h](/src/lib/facade/public/contexts/UltralyticsObbExportContext.h) | `IADataExportersContext` розкладки орієнтованих обмежувальних рамок Ultralytics YOLO |
| [contexts/UltralyticsSegmentExportContext.h](/src/lib/facade/public/contexts/UltralyticsSegmentExportContext.h) | `IADataExportersContext` розкладки сегментації Ultralytics YOLO |
| [contexts/CocoExportContext.h](/src/lib/facade/public/contexts/CocoExportContext.h) | `IADataExportersContext` розкладки детекції об'єктів COCO |
| [contexts/PascalVocExportContext.h](/src/lib/facade/public/contexts/PascalVocExportContext.h) | `IADataExportersContext` розкладки набору даних Pascal VOC |
| [contexts/CreateMLExportContext.h](/src/lib/facade/public/contexts/CreateMLExportContext.h) | `IADataExportersContext` розкладки набору даних для детекції об'єктів Create ML |
| [contexts/Dxxwjz2ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2ExportContext.h) | `IADataExportersContext` розкладки тренувальної директорії darknetxx, тієї, що з мережею для тренування dxxwjz2 |
| [contexts/Dxxwjz3ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3ExportContext.h) | `IADataExportersContext` розкладки тренувальної директорії darknetxx, тієї, що з мережею для тренування dxxwjz3 |
| [contexts/Dxxwjz2pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2pExportContext.h) | `IADataExportersContext` розкладки тренувального проекту dxxwjz2p від darknetxx |
| [contexts/Dxxwjz3pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3pExportContext.h) | `IADataExportersContext` розкладки тренувального проекту dxxwjz3p від darknetxx |
| [contexts/PyTorchExportContext.h](/src/lib/facade/public/contexts/PyTorchExportContext.h) | `IADataExportersContext` розкладки PyTorch Vision разом із її обрізачем зображень |

Кожен із тих тринадцяти дістається шістьох вище через підключення з `../`, і саме це лишає їх розв'язними після встановлення: коренем підключення споживача є директорія, яка містить `ImagesAnnotatorDataExporters-0.12/`, тож звичайне `#include "IADataExportersContext.h"` зсередини `contexts/` шукало б його у тому корені й не знайшло б. Споживача це не стосується: він дістається всіх тринадцяти через `IADataExportersFacade.h`, який їх підключає, і лише проект, що виписує заголовок розкладки напряму, називає складову `contexts/` сам.

[src/lib/facade/CMakeLists.txt](/src/lib/facade/CMakeLists.txt) встановлює директорію цілком під `include/${PROJECT_LIBRARY_NAME}`, що для поточної назви і версії дає `include/ImagesAnnotatorDataExporters-0.12/`. І ту піддиректорію, і звичайний корінь підключення експортує ціль бібліотеки, тож споживач може писати будь-яку з двох форм:

```cpp
#include <ImagesAnnotatorDataExporters-0.12/IADataExportersFacade.h>  // рекомендована
#include <IADataExportersFacade.h>                                    // теж працює
```

Форма з префіксом є безпечною - імена на кшталт `IExporter.h` достатньо загальні, щоб зіткнутися у насиченому шляху підключення.

### Чому видно лише ці заголовки

Бібліотека будується з `CXX_VISIBILITY_PRESET hidden`, тож спільний обʼєкт залишають лише сутності, позначені `IADE_API`. Це вимога коректності, а не оптимізація розміру. Бібліотека ImagesAnnotatorDataDrivers, з якою ця лінкується, походить із того самого шаблону проекту і експортує власні символи `default_logger::DefaultLogger` та `project_decls`. Якби обидва набори експортувалися, динамічний компонувальник звʼязав би виклики однієї бібліотеки з визначеннями іншої.

Сама лише видимість лишає одну дірку. Інстанціація `std::make_shared` називає свій клас у власному спотвореному імені й лишається слабкою та експортованою, якою б не була видимість, тож простір імен реалізації тут - `iade0impl`, а не `lib0impl`, який використовує шаблон проекту, а разом із ним і бібліотека драйверів даних. Два класи `LibFactory` не поділяють навіть розкладки vtable, і до перейменування компонувальник справді звʼязував `std::make_shared<lib0impl::LibFactory>()` однієї бібліотеки з визначенням іншої.

Отже, новий публічний клас належить до [src/lib/facade/public](/src/lib/facade/public) - новий контекст розкладки набору даних до її піддиректорії [contexts](/src/lib/facade/public/contexts), звідки він дістається заголовків над собою через `../`, - і має бути позначений `IADE_API`; кожен інший компонент під [src](/src) лишається приватним для спільного обʼєкта, і дістатися до нього можна лише через абстрактні інтерфейси вище.

### Встановлюваний CMake-пакунок

Разом із бінарником і заголовками збірка встановлює CMake-пакунок, згенерований [src/lib/cmake/lib-cmake-module-gen.cmake](/src/lib/cmake/lib-cmake-module-gen.cmake) із [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in). Він потрапляє до `<libdir>/cmake/ImagesAnnotatorDataExporters-0.12/` і складається з трьох файлів: експортовані цілі, `ImagesAnnotatorDataExporters-0.12ConfigVersion.cmake`, записаний `write_basic_package_version_file()` із сумісністю `SameMajorVersion`, та `ImagesAnnotatorDataExporters-0.12Config.cmake`, який виконує `find_dependency()` пакунка драйверів даних перед підключенням цілей - публічні заголовки називають його типи записів, тож його треба розвʼязати першим.

Тому проекту нижче за течією не потрібно нічого більшого за:

```cmake
find_package(ImagesAnnotatorDataExporters-0.12 0.12 REQUIRED)

target_link_libraries(
  your_target
  PRIVATE ImagesAnnotatorDataExporters-0.12::ImagesAnnotatorDataExporters-0.12
)
```

Див. [Встановлення](/doc/sections/uk_UA/7-installing/7-installing.md) та [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).
