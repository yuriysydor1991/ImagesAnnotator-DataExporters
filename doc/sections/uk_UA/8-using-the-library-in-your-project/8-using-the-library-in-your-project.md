# Використання бібліотеки у власному проекті

Бібліотека призначена для споживання іншими проектами: вона не створює власного виконуваного файлу, лише спільний обʼєкт `libImagesAnnotatorDataExporters-0.11.so` разом зі своїми встановлюваними заголовками і CMake-пакунком. Дана секція описує, що має зробити проект нижче за течією, щоб побудуватись проти неї.

## Що потрібно встановити спершу

Перед конфігуруванням проекту-споживача мають бути встановлені дві речі:

- бібліотека [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), яка оголошує базу даних анотацій і записи зображень, що їх читає ця бібліотека - переглянь підсекцію [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md);
- сама ця бібліотека, побудована і встановлена як описано у секції [Встановлення](/doc/sections/uk_UA/7-installing/7-installing.md).

Встановлення розміщує в обраному префіксі наступне:

```
<prefix>/include/ImagesAnnotatorDataExporters-0.11/     публічні заголовки
<prefix>/lib/libImagesAnnotatorDataExporters-0.11.so    спільний обʼєкт, soname .so.0
<prefix>/lib/cmake/ImagesAnnotatorDataExporters-0.11/   файли CMake-пакунка
```

## Пошук пакунка за допомогою CMake

```cmake
cmake_minimum_required(VERSION 3.13)

project(MyExportingTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImagesAnnotatorDataExporters-0.11 0.11 REQUIRED)

add_executable(my-exporting-tool main.cpp)

target_link_libraries(
  my-exporting-tool
  PRIVATE ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11
)
```

І імʼя пакунка, і імпортована ціль несуть мажорну та мінорну версії бібліотеки, тож майбутній випуск можна встановити паралельно з цим. Запит версії звіряється зі встановленим файлом `ImagesAnnotatorDataExporters-0.11ConfigVersion.cmake` за правилом сумісності `SameMajorVersion`.

Встановлений файл конфігурації пакунка, згенерований з [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in), викликає `find_dependency()` для пакунка драйверів даних перед тим як прочитати експортовані цілі. Бібліотека драйверів даних лінкується як `PUBLIC`, оскільки встановлювані заголовки цієї бібліотеки згадують її типи записів, тож лінкування наведеної вище цілі приносить із собою і шлях до заголовків, і спільний обʼєкт драйверів даних. Окремий `find_package()` для драйверів даних у споживачі не потрібен, хоча його виклик і не шкодить.

Стандарт C++ `17` не є опціональним: публічні заголовки і згадані у них записи компілюються як C++17.

## Як вказати CMake префікси встановлення

Коли будь-яку з бібліотек було встановлено поза типовими системними префіксами, вкажи їхні префікси через `CMAKE_PREFIX_PATH` під час конфігурування свого проекту:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH="/opt/iadd;/opt/iade"
```

Ту саму змінну потребує і ця бібліотека під час власного конфігурування, щоб знайти драйвери даних.

## Підключення заголовків

Експортуються два корені підключення, тож компілюються обидва написання:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>  // рекомендовано
#include <LibraryFacade.h>                                    // теж працює
```

Надавай перевагу написанню з префіксом. Імена заголовків на кшталт `LibraryFacade.h`, `LibraryContext.h` і `ILib.h` достатньо загальні щоб зіткнутись у насиченому шляху підключення - бібліотека драйверів даних встановлює заголовки точно з такими іменами, і за наявності обох простих коренів підключення коротке написання підхоплює той із двох, який компілятор побачить першим.

`LibraryFacade.h` підключає кожен інший публічний заголовок бібліотеки, тож зазвичай це єдиний, який згадує споживач.

## Скорочення просторів імен

Інтерфейсні простори імен обох бібліотек несуть номери своїх мажорної і мінорної версій. Признач їм скорочення один раз - і підняття версії залишиться зміною в один рядок:

```cpp
namespace iadd = ImagesAnnotatorDataDrivers011;
namespace iade = ImagesAnnotatorDataExporters011;
```

## Що має надати твій проект

- **Базу даних.** `LibraryContext::set_db_provider()` приймає `ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr`. Зазвичай він походить з `iadd::LibraryFacade::open_annotations_db("project.json")`, але підійде будь-яка реалізація того інтерфейсу - зібрані у памʼяті записи працюють так само добре.
- **Директорію призначення.** Лише експорт YOLO v4 створює її сам. Для розкладок простого тексту і PyTorch Vision директорія `export_path` має існувати до початку експорту.
- **Обрізач зображень, лише для однієї розкладки.** `PyTorchExportLibraryContext` вирізає анотовані прямокутники із зображень, а сама бібліотека не декодує жодного формату зображень. Реалізуй `IImageCropperFacility` над тим набором засобів роботи із зображеннями, який твій проект уже лінкує, і передай примірник через `PyTorchExportLibraryContext::set_cropper()`. Без нього той експорт одразу зазнає невдачі. Підсекція [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md) містить начерк реалізації. Шести іншим розкладкам обрізач не потрібен зовсім.

## Мінімальний споживач

Наведений нижче `main.cpp` відкриває файл проекту і записує його назовні через одноразову точку входу `ILib::perform_export`:

```cpp
#include <ImagesAnnotatorDataDrivers-0.11/LibraryFacade.h>
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>

#include <filesystem>
#include <iostream>
#include <memory>

namespace iadd = ImagesAnnotatorDataDrivers011;
namespace iade = ImagesAnnotatorDataExporters011;

int main()
{
  auto db = iadd::LibraryFacade::open_annotations_db("project.json");

  if (db == nullptr) {
    std::cerr << "fail to open the project file\n";
    return 1;
  }

  auto ctx = iade::LibraryFacade::create_plain_txt_library_context();

  ctx->set_export_path("plain-dataset");
  ctx->set_db_provider(db);

  std::filesystem::create_directories(ctx->get_export_path());

  auto lib = iade::LibraryFacade::create_library(ctx);

  if (lib == nullptr || !lib->perform_export(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported with the library version "
            << iade::LibraryFacade::library_version() << '\n';

  return 0;
}
```

Створи натомість `iade::Yolo4ExportLibraryContext`, один із трьох `iade::Ultralytics*ExportLibraryContext`, `iade::CocoExportLibraryContext` чи `iade::PyTorchExportLibraryContext`, щоб отримати одну з шести інших розкладок, описаних у підсекції [Розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md). Побудова експортера напряму за допомогою `iade::LibraryFacade::create_exporter()` дає той самий результат із дрібнішим контролем - переглянь підсекцію [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md).

## Запуск результату

Обидва спільні обʼєкти мають бути досяжні для динамічного лінкера під час виконання. Коли їх було встановлено у префікс, який система не переглядає за замовчуванням, або зареєструй той префікс (наприклад додавши його до `/etc/ld.so.conf.d/` і виконавши `ldconfig`), або вкажи його у середовищі:

```
LD_LIBRARY_PATH=/opt/iade/lib:/opt/iadd/lib ./my-exporting-tool
```

`iade::LibraryFacade::library_version()` повідомляє версію бінарника, який було справді завантажено, і це найшвидший спосіб зʼясувати, яка саме з кількох встановлених копій дісталась твоїй програмі.

## Опрацьований приклад усередині цього проекту

Компонентний тест `CTEST_Exporters`, [src/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/exporters/tests/component/Exporters/CTEST_Exporters.cpp), лінкує згенеровану спільну бібліотеку і керує нею винятково через встановлювані заголовки, точно як це робить проект нижче за течією - включно з невеликою реалізацією `IImageCropperFacility`. Увімкни його CMake-опцією `ENABLE_COMPONENT_TESTS`, описаною у секції [Побудова проекту](/doc/sections/uk_UA/5-project-build/5-project-build.md).
