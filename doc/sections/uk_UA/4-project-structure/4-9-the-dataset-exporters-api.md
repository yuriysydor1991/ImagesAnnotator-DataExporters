## API експортерів наборів даних

Увесь інтерфейс бібліотеки оголошено заголовковими файлами у [src/lib/facade/public](/src/lib/facade/public), і він встановлюється у субдиректорію `include/ImagesAnnotatorDataExporters-0.11` обраного префіксу встановлення. Нічого іншого зі спільного обʼєкта не виходить: кожен клас реалізації залишається за описаними тут абстрактними інтерфейсами.

Усі встановлювані оголошення живуть у просторі імен `ImagesAnnotatorDataExporters011`. Імʼя несе номери мажорної і мінорної версій бібліотеки (`0.11` дає суфікс `011`), щоб дві версії бібліотеки могли співіснувати в одній одиниці трансляції. Признач йому скорочення один раз у власному коді:

```cpp
namespace iade = ImagesAnnotatorDataExporters011;
```

Записи, які читають експортери, тут не оголошено. Вони походять з бібліотеки [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), інтерфейсним простором імен якої є `ImagesAnnotatorDataDrivers011` (скорочений нижче як `iadd`). Переглянь підсекцію [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md) щодо збіркової сторони тієї залежності.

### Встановлювані заголовкові файли

| Заголовок | Оголошує |
| --- | --- |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | макрос видимості `IADE_API`, яким позначено кожне встановлюване оголошення |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | абстрактний інтерфейс експортера `IExporter` |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | інтерфейс `IImageCropperFacility`, який реалізує проект-споживач |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | клас вхідних і вихідних даних `LibraryContext`, яким керуються обидві точки входу |
| [PlainTxtExportLibraryContext.h](/src/lib/facade/public/PlainTxtExportLibraryContext.h) | нащадка `LibraryContext` розкладки простого тексту |
| [Yolo4ExportLibraryContext.h](/src/lib/facade/public/Yolo4ExportLibraryContext.h) | нащадка `LibraryContext` розкладки YOLO v4 |
| [PyTorchExportLibraryContext.h](/src/lib/facade/public/PyTorchExportLibraryContext.h) | нащадка `LibraryContext` розкладки PyTorch Vision, разом з обрізачем зображень тієї розкладки |
| [ILib.h](/src/lib/facade/public/ILib.h) | абстрактний інтерфейс бібліотеки `ILib` із його методом `perform_export` |
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | клас-фабрику `LibraryFacade`, точку входу бібліотеки |

Підключення `LibraryFacade.h` затягує кожен інший заголовок переліку.

### Розкладки наборів даних

```cpp
class PlainTxtExportLibraryContext : public LibraryContext;
class Yolo4ExportLibraryContext : public LibraryContext;
class PyTorchExportLibraryContext : public LibraryContext;
```

Ці три нащадки `LibraryContext` називають три розкладки наборів даних, які бібліотека здатна записати. Створення одного з них і є вибором розкладки, а бібліотека відображає той тип на експортер, який її записує. Перші два не додають до `LibraryContext` нічого; `PyTorchExportLibraryContext` додає пару `get_cropper()` / `set_cropper()`, оскільки вирізання пікселів робить лише його розкладка. Що кожна з них розміщує на диску, описано у підсекції [Розкладки згенерованих наборів даних](/doc/sections/uk_UA/4-project-structure/4-10-the-produced-dataset-layouts.md).

### LibraryContext

Єдиний клас даних бібліотеки, той самий, яким керуються обидві її точки входу: одноразовий `ILib::perform_export()` і `IExporter::export_db()` збудованого власноруч експортера. Створюй його методом-фабрикою `LibraryFacade` бажаної розкладки - `create_plain_txt_library_context()`, `create_yolo4_library_context()` чи `create_pytorch_library_context()` - або інстанціюй того нащадка самотужки, як це робить споживач, шаблонізований за типом розкладки.

Дані, які він несе, є приватними і доступні лише через методи доступу. Кожен геттер видає `const`-посилання на те, що тримає контекст, кожен сеттер копіює до себе задане значення:

| Методи доступу | Напрямок | Значення |
| --- | --- | --- |
| `get_export_path()`, `set_export_path()` | вх. | директорія призначення експорту, `std::string`, обовʼязкове |
| `get_db_provider()`, `set_db_provider()` | вх. | база даних анотацій, з якої читаються записи, `ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr`, обовʼязкове |
| `get_exporter()`, `set_exporter()` | вих. | примірник експортера, яким виконався останній `perform_export`, `IExporterPtr` |

`LibraryContextPtr` є скороченням для `std::shared_ptr<LibraryContext>`. `ImagesAnnotatorDataDrivers011::IAnnotationsDBPtr` можна передати до `set_db_provider()` напряму, оскільки `IAnnotationsDB` походить від `IImagesPathsDBProvider`.

### PyTorchExportLibraryContext

Єдиний нащадок, який несе власні дані, бо саме розкладка PyTorch Vision вирізає прямокутники із зображень:

| Методи доступу | Напрямок | Значення |
| --- | --- | --- |
| `get_cropper()`, `set_cropper()` | вх. | служба обрізання зображень, `IImageCropperFacilityPtr`. Необовʼязкова у бібліотеці, зібраній з OpenCV, і обовʼязкова у зібраній без нього |

Жодна інша розкладка не несе місця для обрізача, якого вона ніколи не читає. Тож `export_db()` експортера PyTorch Vision, яким керує будь-який інший контекст, не знаходить наданого споживачем обрізача і відкочується до власного обрізача бібліотеки або зазнає невдачі, коли ця збірка не несе жодного.

### IExporter

```cpp
virtual bool export_db(LibraryContextPtr ectx) = 0;
```

Єдиний метод експортера. Він записує названу контекстом базу даних у розкладці, яку той експортер реалізує, і повертає `true`, коли прохід загалом відбувся. Записи, які він не може опрацювати - запис без прямокутників, відсутній файл зображення - пропускаються і повідомляються через журнал бібліотеки, вони не провалюють прохід.

Розкладка, яку називає тип контексту, тут не розглядається: записується розкладка самого експортера, тож навіть базовий клас `LibraryContext`, який не називає жодної власної розкладки, цілком придатний для `export_db()`. Реалізація не повинна тримати контекст поза межами виклику, оскільки контекст, який тримає той експортер, замкнув би цикл вказівників. `IExporterPtr` є скороченням для `std::shared_ptr<IExporter>`.

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

`ImageRecordPtr` та `ImageRecordRectPtr` є вказівниками на записи з `ImagesAnnotatorDataDrivers011`.

### ILib

`ILib::perform_export(LibraryContextPtr ctx)` є одноразовою точкою входу бібліотеки. Він будує експортер розкладки, яку називає контекст, оприлюднює його через `ctx->set_exporter()` і запускає експорт над тим самим контекстом. Він повертає `false`, коли контекст не називає жодної відомої розкладки або сам експорт зазнав невдачі. Проектам, які бажають дрібнішого контролю, варто радше будувати експортер напряму за допомогою `LibraryFacade::create_exporter()` і викликати його `export_db()` - з тим самим контекстом в обох руках.

### LibraryFacade

Клас, що містить лише статичні методи-фабрики, і єдина точка входу, потрібна проекту-споживачу:

| Метод | Повертає |
| --- | --- |
| `create_plain_txt_library_context()` | новий порожній `PlainTxtExportLibraryContextPtr` |
| `create_yolo4_library_context()` | новий порожній `Yolo4ExportLibraryContextPtr` |
| `create_pytorch_library_context()` | новий порожній `PyTorchExportLibraryContextPtr` - той, що несе обрізач |
| `create_default_lib()` | типову реалізацію `ILibPtr` |
| `create_library(LibraryContextPtr ctx)` | реалізацію `ILibPtr`, відповідну для заданого контексту |
| `create_exporter(const LibraryContextPtr& ctx)` | новий `IExporterPtr` для розкладки контексту, або `nullptr` для контексту без відомої розкладки |
| `create_image_cropper()` | обрізач, який бібліотека несе сама, або `nullptr` у збірці без OpenCV |
| `library_version()` | рядок версії використовуваного бінарника бібліотеки |

### Повний приклад

Наведена нижче програма відкриває файл проекту ImagesAnnotator за допомогою бібліотеки драйверів даних і записує його як тренувальну директорію YOLO v4. Переглянь секцію [Використання бібліотеки у власному проекті](/doc/sections/uk_UA/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md) щодо CMake-сторони цього.

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
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <project.json> <export dir>\n";
    return 1;
  }

  auto db = iadd::LibraryFacade::open_annotations_db(argv[1]);

  if (db == nullptr) {
    std::cerr << "fail to open the project file " << argv[1] << '\n';
    return 1;
  }

  std::filesystem::create_directories(argv[2]);

  auto ctx = iade::LibraryFacade::create_yolo4_library_context();

  ctx->set_db_provider(db);
  ctx->set_export_path(argv[2]);

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  if (exporter == nullptr) {
    std::cerr << "no exporter available for the requested layout\n";
    return 1;
  }

  if (!exporter->export_db(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported by the exporters library version "
            << iade::LibraryFacade::library_version() << '\n';

  return 0;
}
```

Виклик `std::filesystem::create_directories` присутній тому, що лише експортер YOLO v4 створює свою директорію призначення самотужки. Два інші формати очікують, що `export_path` уже існує.

### Реалізація обрізача зображень

`PyTorchExportLibraryContext` є єдиною розкладкою, якій потрібен обрізач. Наведений нижче начерк підключає його до тих засобів роботи із зображеннями, які проект-споживач уже має - заміни виклики `my_imaging` власними викликами декодування та кодування:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/IImageCropperFacility.h>

#include <memory>
#include <string>

namespace iade = ImagesAnnotatorDataExporters011;

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

Передай примірник через `PyTorchExportLibraryContext`, і експортер його підхопить:

```cpp
auto ctx = iade::LibraryFacade::create_pytorch_library_context();

ctx->set_cropper(std::make_shared<MyCropper>());
```

Кожному прямокутнику з тим самим іменем анотації всередині одного зображення пропонується той самий `tofpath`, тож обрізач, який має тримати їх усі окремо, має сам зробити шлях унікальним.

### Позначка IADE_API

Кожен встановлюваний клас позначено макросом `IADE_API` із [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h), а бібліотека компілюється з прихованою видимістю символів, щоб зі спільного обʼєкта виходив лише позначений інтерфейс. Це радше вимога коректності, ніж оптимізація розміру: бібліотека драйверів даних, з якою ця лінкується, побудована з того самого проекту-шаблону і несе власні визначення `default_logger::DefaultLogger` і `project_decls`. Якби експортувались обидва набори, динамічний лінкер міг би вільно повʼязати виклики однієї бібліотеки з інакше розташованими класами іншої.

Приховування не сягає фабрики. Інстанціація `std::make_shared` несе імʼя свого класу у власному спотвореному імені й лишається слабкою та експортованою за будь-якої видимості, тому простором імен реалізації тут є `iade0impl`, а не `lib0impl`, який використовує проект-шаблон, а разом із ним і бібліотека драйверів даних.

Робочий споживач усього інтерфейсу зберігається у дереві як компонентний тест `CTEST_Exporters`, [src/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/exporters/tests/component/Exporters/CTEST_Exporters.cpp): він лінкує справжню спільну бібліотеку і керує нею винятково через публічні заголовки, точно як це робить проект нижче за течією.
