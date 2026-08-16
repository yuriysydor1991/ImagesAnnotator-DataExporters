## Публічні інтерфейсні файли бібліотеки

Директорія [src/lib/facade/public](/src/lib/facade/public) містить увесь встановлюваний інтерфейс бібліотеки - тринадцять заголовків, усі у просторі імен `ImagesAnnotatorDataExporters011`:

| Заголовок | Оголошує |
|---|---|
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | статичні методи `LibraryFacade`, точку входу бібліотеки |
| [ILib.h](/src/lib/facade/public/ILib.h) | `ILib::perform_export()` - виконує експорт, описаний `LibraryContext` |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | методи доступу до даних `perform_export()` та `export_db()`, разом із вихідним `get_exporter()` |
| [PlainTxtExportLibraryContext.h](/src/lib/facade/public/PlainTxtExportLibraryContext.h) | `LibraryContext` розкладки простого тексту |
| [Yolo4ExportLibraryContext.h](/src/lib/facade/public/Yolo4ExportLibraryContext.h) | `LibraryContext` розкладки YOLO v4 (darknet) |
| [UltralyticsDetectExportLibraryContext.h](/src/lib/facade/public/UltralyticsDetectExportLibraryContext.h) | `LibraryContext` розкладки детекції Ultralytics YOLO |
| [UltralyticsObbExportLibraryContext.h](/src/lib/facade/public/UltralyticsObbExportLibraryContext.h) | `LibraryContext` розкладки орієнтованих обмежувальних рамок Ultralytics YOLO |
| [UltralyticsSegmentExportLibraryContext.h](/src/lib/facade/public/UltralyticsSegmentExportLibraryContext.h) | `LibraryContext` розкладки сегментації Ultralytics YOLO |
| [CocoExportLibraryContext.h](/src/lib/facade/public/CocoExportLibraryContext.h) | `LibraryContext` розкладки детекції об'єктів COCO |
| [PyTorchExportLibraryContext.h](/src/lib/facade/public/PyTorchExportLibraryContext.h) | `LibraryContext` розкладки PyTorch Vision разом із її обрізачем зображень |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | `IExporter::export_db()` - окремий експортер, використаний самостійно |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | інтерфейс, який реалізує проект-споживач, щоб вирізати зображення |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | макрос видимості `IADE_API` |

[src/lib/facade/CMakeLists.txt](/src/lib/facade/CMakeLists.txt) встановлює директорію цілком під `include/${PROJECT_LIBRARY_NAME}`, що для поточної назви і версії дає `include/ImagesAnnotatorDataExporters-0.11/`. І ту піддиректорію, і звичайний корінь підключення експортує ціль бібліотеки, тож споживач може писати будь-яку з двох форм:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>  // рекомендована
#include <LibraryFacade.h>                                 // теж працює
```

Форма з префіксом є безпечною - імена на кшталт `IExporter.h` достатньо загальні, щоб зіткнутися у насиченому шляху підключення.

### Чому видно лише ці заголовки

Бібліотека будується з `CXX_VISIBILITY_PRESET hidden`, тож спільний обʼєкт залишають лише сутності, позначені `IADE_API`. Це вимога коректності, а не оптимізація розміру. Бібліотека ImagesAnnotatorDataDrivers, з якою ця лінкується, походить із того самого шаблону проекту і експортує власні символи `default_logger::DefaultLogger` та `project_decls`. Якби обидва набори експортувалися, динамічний компонувальник звʼязав би виклики однієї бібліотеки з визначеннями іншої.

Сама лише видимість лишає одну дірку. Інстанціація `std::make_shared` називає свій клас у власному спотвореному імені й лишається слабкою та експортованою, якою б не була видимість, тож простір імен реалізації тут - `iade0impl`, а не `lib0impl`, який використовує шаблон проекту, а разом із ним і бібліотека драйверів даних. Два класи `LibFactory` не поділяють навіть розкладки vtable, і до перейменування компонувальник справді звʼязував `std::make_shared<lib0impl::LibFactory>()` однієї бібліотеки з визначенням іншої.

Отже, новий публічний клас належить до [src/lib/facade/public](/src/lib/facade/public) і має бути позначений `IADE_API`; кожен інший компонент під [src](/src) лишається приватним для спільного обʼєкта, і дістатися до нього можна лише через абстрактні інтерфейси вище.

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
