## API експортерів наборів даних

Увесь інтерфейс бібліотеки оголошено заголовковими файлами у [src/lib/facade/public](/src/lib/facade/public), і він встановлюється у субдиректорію `include/ImagesAnnotatorDataExporters-0.12` обраного префіксу встановлення. Нічого іншого зі спільного обʼєкта не виходить: кожен клас реалізації залишається за описаними тут абстрактними інтерфейсами.

Усі встановлювані оголошення живуть у просторі імен `ImagesAnnotatorDataExporters012`. Імʼя несе номери мажорної і мінорної версій бібліотеки (`0.12` дає суфікс `012`), щоб дві версії бібліотеки могли співіснувати в одній одиниці трансляції. Признач йому скорочення один раз у власному коді:

```cpp
namespace iade = ImagesAnnotatorDataExporters012;
```

Записи, які читають експортери, тут не оголошено. Вони походять з бібліотеки [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), інтерфейсним простором імен якої є `ImagesAnnotatorDataDrivers012` (скорочений нижче як `iadd`). Переглянь підсекцію [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md) щодо збіркової сторони тієї залежності.

### Встановлювані заголовкові файли

| Заголовок | Оголошує |
| --- | --- |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | макрос видимості `IADE_API`, яким позначено кожне встановлюване оголошення |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | абстрактний інтерфейс експортера `IExporter` |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | інтерфейс `IImageCropperFacility`, який реалізує проект-споживач |
| [IADataExportersContext.h](/src/lib/facade/public/IADataExportersContext.h) | клас вхідних і вихідних даних `IADataExportersContext`, яким керуються обидві точки входу |
| [PlainTxtExportContext.h](/src/lib/facade/public/contexts/PlainTxtExportContext.h) | нащадка `IADataExportersContext` розкладки простого тексту |
| [Yolo4ExportContext.h](/src/lib/facade/public/contexts/Yolo4ExportContext.h) | нащадка `IADataExportersContext` розкладки YOLO v4 (darknet) |
| [UltralyticsDetectExportContext.h](/src/lib/facade/public/contexts/UltralyticsDetectExportContext.h) | нащадка `IADataExportersContext` розкладки детекції Ultralytics YOLO |
| [UltralyticsObbExportContext.h](/src/lib/facade/public/contexts/UltralyticsObbExportContext.h) | нащадка `IADataExportersContext` розкладки орієнтованих обмежувальних рамок Ultralytics YOLO |
| [UltralyticsSegmentExportContext.h](/src/lib/facade/public/contexts/UltralyticsSegmentExportContext.h) | нащадка `IADataExportersContext` розкладки сегментації Ultralytics YOLO |
| [CocoExportContext.h](/src/lib/facade/public/contexts/CocoExportContext.h) | нащадка `IADataExportersContext` розкладки детекції об'єктів COCO |
| [PascalVocExportContext.h](/src/lib/facade/public/contexts/PascalVocExportContext.h) | нащадка `IADataExportersContext` розкладки набору даних Pascal VOC |
| [CreateMLExportContext.h](/src/lib/facade/public/contexts/CreateMLExportContext.h) | нащадка `IADataExportersContext` розкладки набору даних для детекції об'єктів Create ML |
| [Dxxwjz2ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2ExportContext.h) | нащадка `IADataExportersContext` розкладки тренувальної директорії darknetxx, тієї, що з мережею для тренування dxxwjz2 |
| [Dxxwjz3ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3ExportContext.h) | нащадка `IADataExportersContext` розкладки тренувальної директорії darknetxx, тієї, що з мережею для тренування dxxwjz3 |
| [Dxxwjz2pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2pExportContext.h) | нащадка `IADataExportersContext` розкладки тренувального проекту dxxwjz2p від darknetxx |
| [Dxxwjz3pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3pExportContext.h) | нащадка `IADataExportersContext` розкладки тренувального проекту dxxwjz3p від darknetxx |
| [PyTorchExportContext.h](/src/lib/facade/public/contexts/PyTorchExportContext.h) | нащадка `IADataExportersContext` розкладки PyTorch Vision, разом з обрізачем зображень тієї розкладки |
| [IADataExportersLib.h](/src/lib/facade/public/IADataExportersLib.h) | абстрактний інтерфейс бібліотеки `IADataExportersLib` із його методом `perform_export` |
| [IADataExportersFacade.h](/src/lib/facade/public/IADataExportersFacade.h) | клас-фабрику `IADataExportersFacade`, точку входу бібліотеки |

Підключення `IADataExportersFacade.h` затягує кожен інший заголовок переліку.

### Розкладки наборів даних

```cpp
class PlainTxtExportContext : public IADataExportersContext;
class Yolo4ExportContext : public IADataExportersContext;
class UltralyticsDetectExportContext : public IADataExportersContext;
class UltralyticsObbExportContext : public IADataExportersContext;
class UltralyticsSegmentExportContext : public IADataExportersContext;
class CocoExportContext : public IADataExportersContext;
class PascalVocExportContext : public IADataExportersContext;
class CreateMLExportContext : public IADataExportersContext;
class Dxxwjz2ExportContext : public IADataExportersContext;
class Dxxwjz3ExportContext : public IADataExportersContext;
class Dxxwjz2pExportContext : public IADataExportersContext;
class Dxxwjz3pExportContext : public IADataExportersContext;
class PyTorchExportContext : public IADataExportersContext;
```

Ці тринадцять нащадків `IADataExportersContext` називають тринадцять розкладок наборів даних, які бібліотека здатна записати. Створення одного з них і є вибором розкладки, а бібліотека відображає той тип на експортер, який її записує. Усі, окрім останнього, не додають до `IADataExportersContext` нічого; `PyTorchExportContext` додає пару `get_cropper()` / `set_cropper()`, оскільки вирізання пікселів робить лише його розкладка.

Шість із них належать до родини YOLO і не є взаємозамінними. `Yolo4ExportContext` записує тренувальну директорію darknet, яка називає свої класи, перелічує свої зображення і несе всю свою мережу у власних файлах, а `Dxxwjz2ExportContext` і `Dxxwjz3ExportContext` записують ту саму директорію з ще одним файлом - мережею darknetxx для тренування. Три `Ultralytics*` записують розкладку, що її запровадив YOLO v5 і читає кожен наступний випуск Ultralytics - v8, v11 і ті, що після них: єдиний дескриптор `data.yaml` над директоріями `images/train` та `labels/train`. Ці три поділяють кожен байт тієї розкладки і відрізняються лише рядком прямокутника у файлі міток, якого очікує тренована задача - детекція, орієнтовані рамки чи сегментація. Що кожна з них розміщує на диску, описано у підсекції [Розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md).

`CocoExportContext` і `PascalVocExportContext` - дві розкладки переліку, які читає щось інше, ніж тренувальний прохід YOLO: дескриптор COCO JSON беруть Detectron2, MMDetection, torchvision, трансформери детекції HuggingFace, CVAT, FiftyOne, Label Studio і Roboflow, а директорію devkit VOC читають `VOCDetection` з torchvision, `XMLDataset` з MMDetection і LabelImg. Обидві ж передають прямокутник недоторканим, тоді як кожна розкладка YOLO спершу ділить його числа на розмір зображення - `bbox` COCO як пікселі `[x, y, ширина, висота]` зображення, `bndbox` VOC як дві кутові точки у тих самих пікселях. `PascalVocExportContext` до того ж є однією з двох розкладок, які записують імʼя анотації, а не його позицію у впорядкованій множині імен, тож додане до проекту імʼя не перенумеровує нічого з експортованого раніше.

`CreateMLExportContext` - єдина розкладка переліку, яка взагалі виводить за межі тренувальних фреймворків: вона записує те, що читає `MLObjectDetector` від Apple, а натренована з неї модель є детектором Core ML, який працює на iPhone. Вона ж є єдиною розкладкою, чиї координати не є ані кутом, ані нормалізованими - `x`/`y` є **центром** рамки у власних пікселях зображення, тобто тим самим діленням навпіл, яке три розкладки `Ultralytics*` виконують перед діленням на розмір зображення, - і другою, поруч із `PascalVocExportContext`, яка записує імʼя класу замість індексу.

`Dxxwjz2ExportContext` - розкладка [darknetxx](https://github.com/yuriysydor1991/darknetxx), перенесеного на C++ тренера darknet, чий `darknetxx-gui` тренує директорію YOLO v4 як є. Поруч із файлами тієї директорії вона записує `cfg/yolov4-obj.dxxwjz2` - файл dxxwjz2 самої лише мережі: стиснутий gzip JSON-документ із текстом cfg та іменами класів під тими індексами, якими їх нумерують файли міток, без жодних ваг. Такий файл darknetxx тренує з нуля без жодного іншого вказаного файлу, а кожен файл ваг, який зберігає тренування, зберігає й ті імена класів. `Dxxwjz3ExportContext` записує ту саму директорію з `cfg/yolov4-obj.dxxwjz3` натомість - файлом dxxwjz3, третьої версії файлів darknetxx: тією самою мережею без ваг, що складається лише з даних JSON, - опції секцій cfg як значення JSON замість тексту cfg, імена класів як масив імен їхніх індексів.

`Dxxwjz2pExportContext` і `Dxxwjz3pExportContext` - два тренувальні проекти darknetxx: жодної директорії YOLO v4, а лише один файл `yolov4-obj.dxxwjz2p` чи `yolov4-obj.dxxwjz3p` з мережею YOLO v4 проекту, іменами її класів та анотаціями її зображень - структурою проекту ImagesAnnotator, яку читає darknetxx, - поруч із директорією `images/` скопійованих зображень і порожньою `backup/`. darknetxx тренує такий проект з нуля, читаючи зображення його анотацій, і оновлює його вагами кожного збереження. Проект dxxwjz2p зберігає мережу як текст cfg, проект dxxwjz3p - як JSON файлу dxxwjz3.

Файл dxxwjz1 від darknetxx не має тут розкладки, бо він зберігає лише ваги мережі, а база даних анотацій їх не має.

### IADataExportersContext

Єдиний клас даних бібліотеки, той самий, яким керуються обидві її точки входу: одноразовий `IADataExportersLib::perform_export()` і `IExporter::export_db()` збудованого власноруч експортера. Створюй його методом-фабрикою `IADataExportersFacade` бажаної розкладки - `create_plain_txt_library_context()`, `create_yolo4_library_context()`, `create_ultralytics_detect_library_context()`, `create_ultralytics_obb_library_context()`, `create_ultralytics_segment_library_context()`, `create_coco_library_context()`, `create_pascal_voc_library_context()`, `create_createml_library_context()`, `create_dxxwjz2_library_context()` чи `create_pytorch_library_context()` - або інстанціюй того нащадка самотужки, як це робить споживач, шаблонізований за типом розкладки.

Дані, які він несе, є приватними і доступні лише через методи доступу. Кожен геттер видає `const`-посилання на те, що тримає контекст, кожен сеттер копіює до себе задане значення:

| Методи доступу | Напрямок | Значення |
| --- | --- | --- |
| `get_export_path()`, `set_export_path()` | вх. | директорія призначення експорту, `std::string`, обовʼязкове |
| `get_db_provider()`, `set_db_provider()` | вх. | база даних анотацій, з якої читаються записи, `ImagesAnnotatorDataDrivers012::IImagesPathsDBProviderPtr`, обовʼязкове |
| `get_exporter()`, `set_exporter()` | вих. | примірник експортера, яким виконався останній `perform_export`, `IExporterPtr` |

`IADataExportersContextPtr` є скороченням для `std::shared_ptr<IADataExportersContext>`. `ImagesAnnotatorDataDrivers012::IAnnotationsDBPtr` можна передати до `set_db_provider()` напряму, оскільки `IAnnotationsDB` походить від `IImagesPathsDBProvider`.

### PyTorchExportContext

Єдиний нащадок, який несе власні дані, бо саме розкладка PyTorch Vision вирізає прямокутники із зображень:

| Методи доступу | Напрямок | Значення |
| --- | --- | --- |
| `get_cropper()`, `set_cropper()` | вх. | служба обрізання зображень, `IImageCropperFacilityPtr`. Необовʼязкова у бібліотеці, зібраній з OpenCV, і обовʼязкова у зібраній без нього |

Жодна інша розкладка не несе місця для обрізача, якого вона ніколи не читає. Тож `export_db()` експортера PyTorch Vision, яким керує будь-який інший контекст, не знаходить наданого споживачем обрізача і відкочується до власного обрізача бібліотеки або зазнає невдачі, коли ця збірка не несе жодного.

### IExporter

```cpp
virtual bool export_db(IADataExportersContextPtr ectx) = 0;
```

Єдиний метод експортера. Він записує названу контекстом базу даних у розкладці, яку той експортер реалізує, і повертає `true`, коли прохід загалом відбувся. Записи, які він не може опрацювати - запис без прямокутників, відсутній файл зображення - пропускаються і повідомляються через журнал бібліотеки, вони не провалюють прохід.

Розкладка, яку називає тип контексту, тут не розглядається: записується розкладка самого експортера, тож навіть базовий клас `IADataExportersContext`, який не називає жодної власної розкладки, цілком придатний для `export_db()`. Реалізація не повинна тримати контекст поза межами виклику, оскільки контекст, який тримає той експортер, замкнув би цикл вказівників. `IExporterPtr` є скороченням для `std::shared_ptr<IExporter>`.

### IImageCropperFacility

Це єдиний інтерфейс, який проекту-споживачу може знадобитись реалізувати самотужки:

```cpp
virtual bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                           std::string& tofpath) = 0;
virtual IImageCropperFacilityPtr clone() = 0;
```

Бібліотека не декодує жодного власного формату зображень, тож єдиний експорт, який має вирізати прямокутник із зображення, просить свого споживача зробити це за допомогою того набору засобів роботи із зображеннями, який той проект уже лінкує.

Бібліотека, зібрана з OpenCV, несе власну реалізацію, тож цей інтерфейс потрібно реалізовувати лише проекту, який бажає власного обрізання, - або тому, що споживає бібліотеку, зібрану без OpenCV. Див. [Вмикання обрізача зображень на OpenCV](/doc/sections/uk_UA/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md).

- `ir` - запис, що називає зображення для читання, через свій `ImageRecord::get_full_path()`.
- `irr` - прямокутник для вирізання, у власних піксельних координатах зображення (`name`, `x`, `y`, `width`, `height`).
- `tofpath` - параметр вхідний і вихідний водночас: бібліотека заповнює його бажаним шляхом файлу призначення, а реалізація може його переписати, наприклад щоб дописати розширення формату, у який вона кодує.
- Поверни `true` щойно обрізане зображення було записано.

`clone()` має створити копію, яка не поділяє з оригіналом жодного змінюваного стану декодування. Експортери, що постачаються сьогодні, викликають лише `crop_out_2_fs()`, але метод є частиною інтерфейсу і має бути реалізований.

`ImageRecordPtr` та `ImageRecordRectPtr` є вказівниками на записи з `ImagesAnnotatorDataDrivers012`.

### IADataExportersLib

`IADataExportersLib::perform_export(IADataExportersContextPtr ctx)` є одноразовою точкою входу бібліотеки. Він будує експортер розкладки, яку називає контекст, оприлюднює його через `ctx->set_exporter()` і запускає експорт над тим самим контекстом. Він повертає `false`, коли контекст не називає жодної відомої розкладки або сам експорт зазнав невдачі. Проектам, які бажають дрібнішого контролю, варто радше будувати експортер напряму за допомогою `IADataExportersFacade::create_exporter()` і викликати його `export_db()` - з тим самим контекстом в обох руках.

### IADataExportersFacade

Клас, що містить лише статичні методи-фабрики, і єдина точка входу, потрібна проекту-споживачу:

| Метод | Повертає |
| --- | --- |
| `create_plain_txt_library_context()` | новий порожній `PlainTxtExportContextPtr` |
| `create_yolo4_library_context()` | новий порожній `Yolo4ExportContextPtr` |
| `create_ultralytics_detect_library_context()` | новий порожній `UltralyticsDetectExportContextPtr` |
| `create_ultralytics_obb_library_context()` | новий порожній `UltralyticsObbExportContextPtr` |
| `create_ultralytics_segment_library_context()` | новий порожній `UltralyticsSegmentExportContextPtr` |
| `create_coco_library_context()` | новий порожній `CocoExportContextPtr` |
| `create_pascal_voc_library_context()` | новий порожній `PascalVocExportContextPtr` |
| `create_createml_library_context()` | новий порожній `CreateMLExportContextPtr` |
| `create_dxxwjz2_library_context()` | новий порожній `Dxxwjz2ExportContextPtr` |
| `create_dxxwjz3_library_context()` | новий порожній `Dxxwjz3ExportContextPtr` |
| `create_dxxwjz2p_library_context()` | новий порожній `Dxxwjz2pExportContextPtr` |
| `create_dxxwjz3p_library_context()` | новий порожній `Dxxwjz3pExportContextPtr` |
| `create_pytorch_library_context()` | новий порожній `PyTorchExportContextPtr` - той, що несе обрізач |
| `create_default_lib()` | типову реалізацію `IADataExportersLibPtr` |
| `create_library(IADataExportersContextPtr ctx)` | реалізацію `IADataExportersLibPtr`, відповідну для заданого контексту |
| `create_exporter(const IADataExportersContextPtr& ctx)` | новий `IExporterPtr` для розкладки контексту, або `nullptr` для контексту без відомої розкладки |
| `create_image_cropper()` | обрізач, який бібліотека несе сама, або `nullptr` у збірці без OpenCV |
| `library_version()` | рядок версії використовуваного бінарника бібліотеки |

### Повний приклад

Наведена нижче програма відкриває файл проекту ImagesAnnotator за допомогою бібліотеки драйверів даних і записує його як тренувальну директорію YOLO v4. Переглянь секцію [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md) щодо CMake-сторони цього.

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
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <project.json> <export dir>\n";
    return 1;
  }

  auto db = iadd::IADataDriversFacade::open_annotations_db(argv[1]);

  if (db == nullptr) {
    std::cerr << "fail to open the project file " << argv[1] << '\n';
    return 1;
  }

  std::filesystem::create_directories(argv[2]);

  auto ctx = iade::IADataExportersFacade::create_yolo4_library_context();

  ctx->set_db_provider(db);
  ctx->set_export_path(argv[2]);

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  if (exporter == nullptr) {
    std::cerr << "no exporter available for the requested layout\n";
    return 1;
  }

  if (!exporter->export_db(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported by the exporters library version "
            << iade::IADataExportersFacade::library_version() << '\n';

  return 0;
}
```

Виклик `std::filesystem::create_directories` присутній тому, що лише експортер YOLO v4, чотири експортери darknetxx, три експортери Ultralytics YOLO, експортер COCO, експортер Pascal VOC та експортер Create ML створюють свою директорію призначення самотужки. Формати простого тексту та PyTorch Vision очікують, що `export_path` уже існує.

### Реалізація обрізача зображень

`PyTorchExportContext` є єдиною розкладкою, якій потрібен обрізач. Наведений нижче начерк підключає його до тих засобів роботи із зображеннями, які проект-споживач уже має - заміни виклики `my_imaging` власними викликами декодування та кодування:

```cpp
#include <ImagesAnnotatorDataExporters-0.12/IImageCropperFacility.h>

#include <memory>
#include <string>

namespace iade = ImagesAnnotatorDataExporters012;

class MyCropper : public iade::IImageCropperFacility
{
 public:
  bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                     std::string& tofpath) override
  {
    if (ir == nullptr || irr == nullptr) {
      return false;
    }

    auto image = my_imaging::load(ir->get_full_path());

    if (!image) {
      return false;
    }

    auto piece =
        my_imaging::crop(image, irr->x, irr->y, irr->width, irr->height);

    // the wanted destination path may be altered before the writing
    tofpath += ".png";

    return my_imaging::store_png(piece, tofpath);
  }

  IImageCropperFacilityPtr clone() override
  {
    return std::make_shared<MyCropper>();
  }
};
```

Передай примірник через `PyTorchExportContext`, і експортер його підхопить:

```cpp
auto ctx = iade::IADataExportersFacade::create_pytorch_library_context();

ctx->set_cropper(std::make_shared<MyCropper>());
```

Кожному прямокутнику з тим самим іменем анотації всередині одного зображення пропонується той самий `tofpath`, тож обрізач, який має тримати їх усі окремо, має сам зробити шлях унікальним.

### Позначка IADE_API

Кожен встановлюваний клас позначено макросом `IADE_API` із [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h), а бібліотека компілюється з прихованою видимістю символів, щоб зі спільного обʼєкта виходив лише позначений інтерфейс. Це радше вимога коректності, ніж оптимізація розміру: бібліотека драйверів даних, з якою ця лінкується, побудована з того самого проекту-шаблону і несе власні визначення `default_logger::DefaultLogger` і `project_decls`. Якби експортувались обидва набори, динамічний лінкер міг би вільно повʼязати виклики однієї бібліотеки з інакше розташованими класами іншої.

Приховування не сягає фабрики. Інстанціація `std::make_shared` несе імʼя свого класу у власному спотвореному імені й лишається слабкою та експортованою за будь-якої видимості, тому простором імен реалізації тут є `iade0impl`, а не `lib0impl`, який використовує проект-шаблон, а разом із ним і бібліотека драйверів даних.

Робочий споживач усього інтерфейсу зберігається у дереві як компонентний тест `CTEST_Exporters`, [src/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/exporters/tests/component/Exporters/CTEST_Exporters.cpp): він лінкує справжню спільну бібліотеку і керує нею винятково через публічні заголовки, точно як це робить проект нижче за течією.
