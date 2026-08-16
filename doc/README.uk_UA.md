**Бібліотека експортерів наборів даних анотацій проекту ImagesAnnotator**

# Що це таке

`ImagesAnnotatorDataExporters` - це спільна бібліотека на C++17, яка перетворює анотації застосунку [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) - анотовані зображення з намальованими поверх них іменованими прямокутниками - у розкладки тренувальних наборів даних, які читають фреймворки машинного навчання.

Раніше цей код жив усередині самого застосунку. Його було виділено в окрему бібліотеку, щоб застосунок ImagesAnnotator і **будь-який інший інструмент** могли створювати один і той самий набір даних із проекту анотацій без дублювання експортерів.

Усе, чого торкається проект-споживач, приховано за абстрактними інтерфейсами заголовків [src/lib/facade/public](/src/lib/facade/public), тож ані клас реалізації, ані жодна зі сторонніх залежностей бібліотеки не просочуються у код нижче за течією.

Більше за посиланням [kytok.org.ua](http://www.kytok.org.ua/)

💵 Підтримай проект за посиланням [http://kytok.org.ua/page/pozertvy](http://kytok.org.ua/page/pozertvy)

# Можливості

- **Три розкладки наборів даних з однієї бази даних** - обираються створеним нащадком `LibraryContext` і реалізовані окремим класом-експортером кожна:
  - `PlainTxtExportLibraryContext` - по одному файлу `<імʼя-анотації>.txt` на кожне імʼя анотації, де кожен рядок називає зображення і його прямокутники;
  - `Yolo4ExportLibraryContext` - ціла тренувальна директорія darknet для детектора YOLO v4: `data/obj.names`, `data/obj.data`, опис `cfg/yolov4-obj.cfg` цілої 162-шарової мережі YOLO v4, записаний під класи проекту, скопійовані зображення з їхніми нормалізованими файлами міток `.txt`, переліки `train.txt` і `val.txt` та порожня `backup/`;
  - `PyTorchExportLibraryContext` - класифікаційна розкладка, яку читає набір даних `ImageFolder` з PyTorch Vision: одна директорія на імʼя анотації, що містить зображення, обрізані до прямокутників із тим іменем.
- **Одноразова точка входу** - заповни нащадка `LibraryContext` потрібної розкладки директорією призначення і базою даних, і `ILib::perform_export()` побудує потрібний експортер та запустить його. `LibraryFacade::create_exporter()` дає той самий результат із дрібнішим контролем.
- **Вебзображення попередньо звантажуються** - запис, який вказує на вебсторінку, звантажується за допомогою [libcurl](https://curl.se/libcurl/) у тимчасовий кеш попереднього завантаження ще до того як експорт його торкнеться, тож проект, що змішує локальні й віддалені зображення, експортується як одне ціле.
- **Стійкість до неповної бази даних** - запис без прямокутників, із нульовим розміром чи з непридатним до читання файлом зображення потрапляє до журналу і пропускається, а сам прохід експорту триває далі.
- **Жодного власного кодека зображень** - бібліотека копіює файли зображень як є. Єдиний формат, який має вирізати прямокутники, просить проект-споживач зробити це через інтерфейс `IImageCropperFacility`, за допомогою тих засобів роботи із зображеннями, які той проект уже лінкує.
- **Версійований встановлюваний інтерфейс** - простір імен, бінарник, директорія заголовків і CMake-пакунок усі несуть пару мажорної й мінорної версій `0.11`, тож два мінорні випуски встановлюються паралельно.

# Приклад використання

Бібліотека споживається через CMake-пакунок. Наведену нижче програму було скомпільовано, злінковано і запущено проти встановленої бібліотеки:

```cmake
cmake_minimum_required(VERSION 3.13)
project(MyTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImagesAnnotatorDataExporters-0.11 REQUIRED)

add_executable(mytool main.cpp)
target_link_libraries(mytool ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11)
```

```cpp
#include <ImagesAnnotatorDataDrivers-0.11/LibraryFacade.h>
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>

#include <filesystem>
#include <iostream>
#include <memory>

namespace iadd = ImagesAnnotatorDataDrivers011;
namespace iade = ImagesAnnotatorDataExporters011;

int main(int argc, char** argv)
{
  if (argc < 3) { return 1; }

  auto db = iadd::LibraryFacade::open_annotations_db(argv[1]);

  if (db == nullptr) { return 1; }

  auto ctx = std::make_shared<iade::Yolo4ExportLibraryContext>();

  ctx->set_export_path(argv[2]);
  ctx->set_db_provider(db);

  std::filesystem::create_directories(ctx->get_export_path());

  auto lib = iade::LibraryFacade::create_default_lib();

  if (lib == nullptr || !lib->perform_export(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported with " << iade::LibraryFacade::library_version() << "\n";

  return 0;
}
```

Імʼя простору імен `ImagesAnnotatorDataExporters011` навмисно несе номери мажорної й мінорної версій бібліотеки: дві версії бібліотеки можуть співіснувати всередині однієї одиниці трансляції без жодного зіткнення символів. Признач йому скорочення, як показано вище, і підняття версії залишиться зміною в один рядок на твоєму боці.

Для встановленого споживача працюють обидва написання - і `#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>`, і просте `#include <LibraryFacade.h>`, оскільки бібліотека експортує корінь підключення разом зі своєю версійованою субдиректорією. Рекомендованим є написання з префіксом: імена заголовків на кшталт `LibraryFacade.h`, `LibraryContext.h` чи `ILib.h` достатньо загальні щоб зіткнутись у насиченому шляху підключення - бібліотека драйверів даних встановлює заголовки точно з такими іменами.

Більше про API і про згенеровані набори даних - у секціях документації [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md), [розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md) і [використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).

# Залежності

Обидві наведені нижче є **обовʼязковими** для побудови бібліотеки:

| CMake-опція | Бібліотека | Навіщо вона потрібна |
| --- | --- | --- |
| (завжди увімкнено) | [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) | вона оголошує базу даних анотацій і записи зображень, які читають експортери |
| `ENABLE_LIBCURL` | [libcurl](https://curl.se/libcurl/) | вона попередньо звантажує записи зображень, які вказують на вебадресу замість локального файлу |

libcurl спершу шукається у системі, і коли її не знайдено - затягується через CMake `FetchContent`; переглянь секцію [вмикання інтеграції libcurl](/doc/sections/uk_UA/5-project-build/5-14-enabling-libcurl.md) щодо змінних заміни джерела і теґу. Вона лінкується **приватно**: жоден публічний заголовок не відкриває curl-типів.

Бібліотека драйверів даних відрізняється. Вона має бути встановлена заздалегідь, знаходиться через `find_package(ImagesAnnotatorDataDrivers-0.11 REQUIRED CONFIG)` і лінкується **публічно**, оскільки встановлювані заголовки цієї бібліотеки згадують її типи записів. Вкажи конфігуруванню її префікс встановлення за допомогою `-DCMAKE_PREFIX_PATH=<prefix>`, коли вона не міститься у типовому системному префіксі. Секція [залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md) розкриває це повністю, включно з іменем пакунка.

Компонент журналювання компілюється прямо у спільну бібліотеку, тож споживач не має надавати жодної реалізації журналювання.

# Побудова і тестування

Звичайна побудова, проти встановлення драйверів даних у `$HOME/iadd-install`:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/iadd-install
cmake --build build -j$(nproc)
```

Тести вимкнено за замовчуванням. Щоб побудувати і запустити їх:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/iadd-install \
  -DENABLE_UNIT_TESTS=ON -DENABLE_COMPONENT_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

З обома увімкненими опціями набір містить 61 тестовий випадок. Цілі `ENABLE_UNIT_TESTS` компілюються прямо із сирців проти gmock-замінників з [src/tests/mocks](/src/tests/mocks), тоді як `CTEST_Exporters` з `ENABLE_COMPONENT_TESTS` лінкує справжню спільну бібліотеку і керує нею винятково через публічні заголовки - точно як це робить проект нижче за течією.

Встановлення - звичайне `sudo cmake --install build`, докладно описане у секції [встановлення](/doc/sections/uk_UA/7-installing/7-installing.md).

# Звідки походить код

Сирці було виділено із застосунку [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git), а саме з його компонента `src/annotator-business/exporters`, разом із замиканням залежностей, якого він потребує - `ImageLoader`, `ImageRecordUrlAndPathHelper`, `TypeHelper` і `CURLController`. Застосунок має позбутись власної копії і споживати цю бібліотеку натомість.

Сама база даних анотацій - розбирач файлу проекту, серіалізатор і правила злиття - **не** є частиною цієї бібліотеки. Вона живе у спорідненому проекті [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), через який ця читає свої записи.

Переглянь директорію `doc` щодо можливих перекладів поточного md-документа:
- `en_US` за відносною адресою [README.md](/README.md)

# Зміст документації

**Даний документ у процесі покращення**

1. [Вимоги](/doc/sections/uk_UA/3-requirements/3-requirements.md)
    1. [Обов'язкові інструменти для ОС на базі GNU/Лінукс](/doc/sections/uk_UA/3-requirements/3-1-required-tools-for-the-GNU-Linux-based-OS.md)
    1. [Обов'язкові інструменти для ОС на базі MS Windows](/doc/sections/uk_UA/3-requirements/3-2-required-tools-for-the-MS-Windows-based-OS.md)
    1. [Необов'язкові пакети для тестів](/doc/sections/uk_UA/3-requirements/3-3-optional-for-the-tests.md)
    1. [Необов'язкові пакети для створення документації](/doc/sections/uk_UA/3-requirements/3-4-optional-for-the-documentation.md)
    1. [Необов'язкові пакети для форматування коду](/doc/sections/uk_UA/3-requirements/3-5-optional-for-the-code-formatting.md)
    1. [Необов'язкові пакети для статичного аналізатора коду cppcheck](/doc/sections/uk_UA/3-requirements/3-6-optional-for-the-code-analyzer-cppcheck.md)
    1. [Необов'язкові пакети для статичного аналізатора коду clang-tidy](/doc/sections/uk_UA/3-requirements/3-7-optional-for-the-code-analyzer-with-clang-tidy.md)
1. [Структура проекту](/doc/sections/uk_UA/4-project-structure/4-project-structure.md)
    1. [Діаграми проекту](/doc/sections/uk_UA/4-project-structure/4-0-project-diagrams.md)
    1. [Реалізуй код одразу!](/doc/sections/uk_UA/4-project-structure/4-1-implement-code-straight-away.md)
    1. [Публічні інтерфейсні файли бібліотеки](/doc/sections/uk_UA/4-project-structure/4-8-the-librarys-installable-include-header-files.md)
    1. [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md)
    1. [Розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md)
    1. [Версіювання і інші параметри проекту](/doc/sections/uk_UA/4-project-structure/4-3-version-tracking-and-other-project-parameters.md)
    1. [Тести проекту](/doc/sections/uk_UA/4-project-structure/4-4-project-tests.md)
        1. [Фреймворк тестів Google Test](/doc/sections/uk_UA/4-project-structure/4-4-1-google-test.md)
1. [Побудова проекту](/doc/sections/uk_UA/5-project-build/5-project-build.md)
    1. [Побудова за допомогою IDE](/doc/sections/uk_UA/5-project-build/5-1-IDE-build.md)
    1. [Побудова проекту через командний рядок](/doc/sections/uk_UA/5-project-build/5-2-command-line-build.md)
    1. [Швидкі скрипти побудови](/doc/sections/uk_UA/5-project-build/5-38-quick-build-scripts.md)
    1. [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md)
    1. Вмикання тестів
        1. [Вмикання юніт-тестів](/doc/sections/uk_UA/5-project-build/testing/5-3-1-enabling-unit-testing.md)
        1. [Запобігання використання GTest з ОС](/doc/sections/uk_UA/5-project-build/testing/5-3-2-disabling-system-GTest-probe.md)
    1. [Побудова документації](/doc/sections/uk_UA/5-project-build/documentation/5-4-documentation-build.md)
    1. [Вмикання підтримки встановлення документації](/doc/sections/uk_UA/5-project-build/documentation/5-5-configuring-the-documentation-install-support.md)
    1. [Налаштування складових імені встановлюваної бібліотеки](/doc/sections/uk_UA/5-project-build/5-23-customizing-library-name-segments.md)
    1. Якість коду та санітайзери
        1. [Вмикання підтримки форматування коду](/doc/sections/uk_UA/5-project-build/code-quality/5-6-enabling-and-performing-code-formatting-target.md)
        1. [Вмикання підтримки цілі статичного аналізатора коду cppcheck](/doc/sections/uk_UA/5-project-build/code-quality/5-7-enabling-the-static-code-analyzer-target-with-cppcheck.md)
        1. [Вмикання підтримки статичного аналізатора коду clang-tidy](/doc/sections/uk_UA/5-project-build/code-quality/5-8-enabling-static-code-analyzer-with-clang-tidy.md)
    1. Контейнери та CI
        1. [Вмикання конвеєра Jenkins усередині Docker-контейнера](/doc/sections/uk_UA/5-project-build/containers-ci/5-17-enabling-Jenkins-pipeline-inside-Docker-container.md)
    1. Пакувальники
        1. [Вмикання підтримки генерування DEB-пакетів з cpack](/doc/sections/uk_UA/5-project-build/packagers/5-10-enabling-DEB-package-generation-with-cpack.md)
        1. [Вмикання підтримки генерування пакунків FreeBSD pkg з cpack](/doc/sections/uk_UA/5-project-build/packagers/5-20-enabling-FreeBSD-pkg-package-generation-with-cpack.md)
        1. [Вмикання підтримки генерування WIX MSI-пакетів з cpack](/doc/sections/uk_UA/5-project-build/packagers/5-21-enabling-WIX-MSI-package-generation-with-cpack.md)
        1. [Вмикання підтримки генерування RPM-пакунків з cpack](/doc/sections/uk_UA/5-project-build/packagers/5-22-enabling-RPM-package-generation-with-cpack.md)
    1. Бібліотеки
        1. [Вмикання інтеграції libcurl](/doc/sections/uk_UA/5-project-build/5-14-enabling-libcurl.md)
        1. [Вмикання обрізача зображень на OpenCV](/doc/sections/uk_UA/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md)
1. Запуск тестів
    1. [Запуск тестів за допомогою ctest](/doc/sections/uk_UA/6-running-the-tests/6-3-1-run-tests-by-the-ctest.md)
    1. [Ручний запуск тестів](/doc/sections/uk_UA/6-running-the-tests/6-3-2-manual-tests-run.md)
1. [Встановлення](/doc/sections/uk_UA/7-installing/7-installing.md)
1. [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md)
