**Бібліотека експортерів наборів даних анотацій проекту ImagesAnnotator**

# Що це таке

`ImagesAnnotatorDataExporters` - це спільна бібліотека на C++17, яка перетворює анотації застосунку [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) - анотовані зображення з намальованими поверх них іменованими прямокутниками - у розкладки тренувальних наборів даних, які читають фреймворки машинного навчання.

Раніше цей код жив усередині самого застосунку. Його було виділено в окрему бібліотеку, щоб застосунок ImagesAnnotator і **будь-який інший інструмент** могли створювати один і той самий набір даних із проекту анотацій без дублювання експортерів.

Усе, чого торкається проект-споживач, приховано за абстрактними інтерфейсами заголовків [src/lib/facade/public](/src/lib/facade/public), тож ані клас реалізації, ані жодна зі сторонніх залежностей бібліотеки не просочуються у код нижче за течією.

Більше за посиланням [kytok.org.ua](http://www.kytok.org.ua/)

💵 Підтримай проект за посиланням [http://kytok.org.ua/page/pozertvy](http://kytok.org.ua/page/pozertvy)

# Можливості

- **Тринадцять розкладок наборів даних з однієї бази даних** - обираються створеним нащадком `IADataExportersContext` і реалізовані окремим класом-експортером кожна:
  - **Звичайний текст** (`PlainTxtExportContext`) - по одному файлу `<імʼя-анотації>.txt` на кожне імʼя анотації, де кожен рядок називає зображення і його прямокутники;
  - **YOLO v4 (darknet)** (`Yolo4ExportContext`) - ціла тренувальна директорія darknet для детектора YOLO v4: `data/obj.names`, `data/obj.data`, опис `cfg/yolov4-obj.cfg` цілої 162-шарової мережі YOLO v4, записаний під класи проекту, скопійовані зображення з їхніми нормалізованими файлами міток `.txt`, переліки `train.txt` і `val.txt` та порожня `backup/`;
  - **Ultralytics YOLO - детекція** (`UltralyticsDetectExportContext`) - набір даних для детекції, який читає кожен випуск Ultralytics, розкладка, що її запровадив YOLO v5, а v8, v11 і наступні за ними зберегли: єдиний дескриптор `data.yaml` над директоріями `images/train` та `labels/train`, один рядок `клас центр-x центр-y ширина висота` на прямокутник;
  - **Ultralytics YOLO - орієнтовані обмежувальні рамки** (`UltralyticsObbExportContext`) - та сама директорія і той самий дескриптор для задачі орієнтованих обмежувальних рамок, один рядок `клас x1 y1 x2 y2 x3 y3 x4 y4` із чотирьох кутів рамки на прямокутник;
  - **Ultralytics YOLO - сегментація** (`UltralyticsSegmentExportContext`) - та сама директорія і той самий дескриптор для задачі сегментації екземплярів, один рядок-багатокутник `клас x1 y1 ... xn yn` на прямокутник;
  - **COCO JSON** (`CocoExportContext`) - набір даних для детекції об'єктів COCO, який читають Detectron2, MMDetection, torchvision, трансформери детекції HuggingFace, CVAT, FiftyOne, Label Studio і Roboflow: скопійовані зображення і єдиний дескриптор `annotations/instances_default.json` над ними, що несе кожен прямокутник як пікселі `[x, y, ширина, висота]` того зображення, на якому його намалювали, замість нормалізованих чисел, яких вимагає кожна розкладка YOLO вище;
  - **Pascal VOC XML** (`PascalVocExportContext`) - набір даних Pascal VOC у формі директорій devkit, ті самі файли, які записує і читає [LabelImg](https://github.com/HumanSignal/labelImg), і те, що беруть `VOCDetection` з torchvision та `XMLDataset` з MMDetection: скопійовані зображення під `JPEGImages/`, по одному дескриптору `Annotations/<основа>.xml` на зображення над ними і переліки `ImageSets/Main`, що їх називають, де кожен прямокутник є двома кутовими точками, між якими його намалювали, а імʼя його класу записане як є, а не як індекс;
  - **Create ML JSON** (`CreateMLExportContext`) - набір даних для детекції об'єктів Create ML, шлях від проекту до детектора Core ML, що працює на iPhone: скопійовані зображення і єдиний дескриптор `annotations.json` поруч із ними в одній пласкій директорії, що є джерелом даних [`directoryWithImagesAndJsonAnnotation`](https://developer.apple.com/documentation/createml/building-an-object-detector-data-source) з `MLObjectDetector` від Apple і тією самою текою, яку бере застосунок Create ML, коли її кидають до його тренувального лотка, де кожен прямокутник несе піксельний **центр** рамки поруч із її розміром - те саме ділення навпіл, яке виконує кожна розкладка YOLO вище, лише лишене в пікселях зображення замість нормалізованого;
  - **darknetxx dxxwjz2** (`Dxxwjz2ExportContext`) - описана вище тренувальна директорія darknet для YOLO v4, яку `darknetxx-gui` з [darknetxx](https://github.com/yuriysydor1991/darknetxx) тренує як є, з мережею для тренування `cfg/yolov4-obj.dxxwjz2` поруч із її файлом cfg: стиснутим gzip JSON-документом darknetxx, що несе текст cfg та імена класів під тими індексами, якими їх нумерують файли міток, і не несе жодних ваг, - `darknetxx --train-cfg cfg/yolov4-obj.dxxwjz2` тренує її з нуля без жодного іншого вказаного файлу;
  - **darknetxx dxxwjz3** (`Dxxwjz3ExportContext`) - та сама директорія з мережею для тренування `cfg/yolov4-obj.dxxwjz3` натомість: файлом dxxwjz3 з darknetxx, документом лише з даних JSON, що несе опції секцій cfg як значення JSON замість тексту cfg та імена класів як масив імен їхніх індексів, - тим самим файлом, який конвертер darknetxx створює з cfg та імен класів директорії, окрім generator;
  - **darknetxx dxxwjz2p project** (`Dxxwjz2pExportContext`) - тренувальний проект darknetxx: єдиний файл `yolov4-obj.dxxwjz2p` з мережею YOLO v4 проекту, іменами її класів та анотаціями зображень у структурі проекту ImagesAnnotator, поруч із директорією `images/` скопійованих зображень та порожньою `backup/`, який `darknetxx --train-cfg yolov4-obj.dxxwjz2p` і `darknetxx-train-gui` тренують з нуля, оновлюючи проект вагами;
  - **darknetxx dxxwjz3p project** (`Dxxwjz3pExportContext`) - той самий проект у файлі `yolov4-obj.dxxwjz3p`, мережа якого є мережею JSON файлу dxxwjz3;
  - **PyTorch Vision `ImageFolder`** (`PyTorchExportContext`) - класифікаційна розкладка, яку читає набір даних `ImageFolder` з PyTorch Vision: одна директорія на імʼя анотації, що містить зображення, обрізані до прямокутників із тим іменем.
- **Одноразова точка входу** - заповни нащадка `IADataExportersContext` потрібної розкладки директорією призначення і базою даних, і `IADataExportersLib::perform_export()` побудує потрібний експортер та запустить його. `IADataExportersFacade::create_exporter()` дає той самий результат із дрібнішим контролем.
- **Вебзображення попередньо звантажуються** - запис, який вказує на вебсторінку, звантажується за допомогою [libcurl](https://curl.se/libcurl/) у тимчасовий кеш попереднього завантаження ще до того як експорт його торкнеться, тож проект, що змішує локальні й віддалені зображення, експортується як одне ціле.
- **Стійкість до неповної бази даних** - запис без прямокутників, із нульовим розміром чи з непридатним до читання файлом зображення потрапляє до журналу і пропускається, а сам прохід експорту триває далі.
- **Жодного власного кодека зображень** - бібліотека копіює файли зображень як є. Єдиний формат, який має вирізати прямокутники, просить проект-споживач зробити це через інтерфейс `IImageCropperFacility`, за допомогою тих засобів роботи із зображеннями, які той проект уже лінкує.
- **Версійований встановлюваний інтерфейс** - простір імен, бінарник, директорія заголовків і CMake-пакунок усі несуть пару мажорної й мінорної версій `0.12`, тож два мінорні випуски встановлюються паралельно.

# Приклад використання

Бібліотека споживається через CMake-пакунок. Наведену нижче програму було скомпільовано, злінковано і запущено проти встановленої бібліотеки:

```cmake
cmake_minimum_required(VERSION 3.13)
project(MyTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImagesAnnotatorDataExporters-0.12 REQUIRED)

add_executable(mytool main.cpp)
target_link_libraries(mytool ImagesAnnotatorDataExporters-0.12::ImagesAnnotatorDataExporters-0.12)
```

```cpp
#include <ImagesAnnotatorDataDrivers-0.12/IADataDriversFacade.h>
#include <ImagesAnnotatorDataExporters-0.12/IADataExportersFacade.h>

#include <filesystem>
#include <iostream>
#include <memory>

namespace iadd = ImagesAnnotatorDataDrivers012;
namespace iade = ImagesAnnotatorDataExporters012;

int main(int argc, char** argv)
{
  if (argc < 3) { return 1; }

  auto db = iadd::IADataDriversFacade::open_annotations_db(argv[1]);

  if (db == nullptr) { return 1; }

  auto ctx = iade::IADataExportersFacade::create_yolo4_library_context();

  ctx->set_export_path(argv[2]);
  ctx->set_db_provider(db);

  std::filesystem::create_directories(ctx->get_export_path());

  auto lib = iade::IADataExportersFacade::create_default_lib();

  if (lib == nullptr || !lib->perform_export(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported with " << iade::IADataExportersFacade::library_version() << "\n";

  return 0;
}
```

Імʼя простору імен `ImagesAnnotatorDataExporters012` навмисно несе номери мажорної й мінорної версій бібліотеки: дві версії бібліотеки можуть співіснувати всередині однієї одиниці трансляції без жодного зіткнення символів. Признач йому скорочення, як показано вище, і підняття версії залишиться зміною в один рядок на твоєму боці.

Для встановленого споживача працюють обидва написання - і `#include <ImagesAnnotatorDataExporters-0.12/IADataExportersFacade.h>`, і просте `#include <IADataExportersFacade.h>`, оскільки бібліотека експортує корінь підключення разом зі своєю версійованою субдиректорією. Рекомендованим є написання з префіксом: `IExporter.h` і `ExportersAPI.h` достатньо загальні щоб зіткнутись у насиченому шляху підключення, а префікс закріплює версію бібліотеки, з якою розмовляє одиниця трансляції.

Більше про API і про згенеровані набори даних - у секціях документації [API експортерів наборів даних](/doc/sections/uk_UA/4-project-structure/4-9-the-dataset-exporters-api.md), [розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md) і [використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).

# Залежності

Усі три наведені нижче є **обовʼязковими** для побудови бібліотеки:

| CMake-опція | Бібліотека | Навіщо вона потрібна |
| --- | --- | --- |
| (завжди увімкнено) | [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) | вона оголошує базу даних анотацій і записи зображень, які читають експортери |
| `ENABLE_LIBCURL` | [libcurl](https://curl.se/libcurl/) | вона попередньо звантажує записи зображень, які вказують на вебадресу замість локального файлу |
| `ENABLE_ZLIB` | [zlib](https://zlib.net/) | вона компресує gzip-файли dxxwjz [darknetxx](https://github.com/yuriysydor1991/darknetxx), які пишуть розкладки darknetxx |

libcurl спершу шукається у системі, і коли її не знайдено - затягується через CMake `FetchContent`; переглянь секцію [вмикання інтеграції libcurl](/doc/sections/uk_UA/5-project-build/5-14-enabling-libcurl.md) щодо змінних заміни джерела і теґу. Вона лінкується **приватно**: жоден публічний заголовок не відкриває curl-типів.

zlib шукається і завантажується так само і так само лінкується **приватно**. Бібліотека звертається до неї через `ZlibController` з [src/zlib](/src/zlib), компонент контролера zlib з гілки `appZlib` [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template) в адаптації, яку зробив з нього darknetxx - див. секцію [вмикання бібліотеки zlib](/doc/sections/uk_UA/5-project-build/5-39-enabling-the-zlib-library.md).

Бібліотека драйверів даних відрізняється. Вона має бути встановлена заздалегідь, знаходиться через `find_package(ImagesAnnotatorDataDrivers-0.12 REQUIRED CONFIG)` і лінкується **публічно**, оскільки встановлювані заголовки цієї бібліотеки згадують її типи записів. Вкажи конфігуруванню її префікс встановлення за допомогою `-DCMAKE_PREFIX_PATH=<prefix>`, коли вона не міститься у типовому системному префіксі. Секція [залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md) розкриває це повністю, включно з іменем пакунка.

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

З обома увімкненими опціями набір містить 246 тестових випадків. Цілі `ENABLE_UNIT_TESTS` компілюються прямо із сирців проти gmock-замінників з [src/tests/mocks](/src/tests/mocks), тоді як `CTEST_Exporters` з `ENABLE_COMPONENT_TESTS` лінкує справжню спільну бібліотеку і керує нею винятково через публічні заголовки - точно як це робить проект нижче за течією.

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
    1. [Де живе реалізація експортерів](/doc/sections/uk_UA/4-project-structure/4-1-implement-code-straight-away.md)
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
        1. [Вмикання бібліотеки zlib](/doc/sections/uk_UA/5-project-build/5-39-enabling-the-zlib-library.md)
        1. [Вмикання обрізача зображень на OpenCV](/doc/sections/uk_UA/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md)
1. Запуск тестів
    1. [Запуск тестів за допомогою ctest](/doc/sections/uk_UA/6-running-the-tests/6-3-1-run-tests-by-the-ctest.md)
    1. [Ручний запуск тестів](/doc/sections/uk_UA/6-running-the-tests/6-3-2-manual-tests-run.md)
1. [Встановлення](/doc/sections/uk_UA/7-installing/7-installing.md)
1. [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md)
