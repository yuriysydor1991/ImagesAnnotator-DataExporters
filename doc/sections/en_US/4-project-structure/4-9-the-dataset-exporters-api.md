## The dataset exporters API

The whole interface of the library is declared by the header files under [src/lib/facade/public](/src/lib/facade/public) and gets installed into the `include/ImagesAnnotatorDataExporters-0.11` sub-directory of the chosen install prefix. Nothing else leaves the shared object: every implementation class stays behind the abstract interfaces described here.

All the installable declarations live in the `ImagesAnnotatorDataExporters011` namespace. The name carries the library major and minor version numbers (`0.11` gives the `011` suffix) so that two library versions may coexist in a single translation unit. Alias it once in your own code:

```cpp
namespace iade = ImagesAnnotatorDataExporters011;
```

The records the exporters read are not defined here. They come from the [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) library, whose interface namespace is `ImagesAnnotatorDataDrivers011` (aliased below as `iadd`). See the [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md) subsection for the build side of that dependency.

### The installable header files

| Header | Declares |
| --- | --- |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | the `IADE_API` visibility macro every installable declaration is marked with |
| [ExportFormat.h](/src/lib/facade/public/ExportFormat.h) | the `ExportFormat` enumeration of the supported dataset layouts |
| [ExportContext.h](/src/lib/facade/public/ExportContext.h) | the `ExportContext` data class an exporter is driven with |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | the `IExporter` abstract exporter interface |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | the `IImageCropperFacility` interface the consuming project implements |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | the `LibraryContext` in and out data class of the single shot entry point |
| [ILib.h](/src/lib/facade/public/ILib.h) | the `ILib` abstract library interface with its `libcall` method |
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | the `LibraryFacade` factory class, the entry point of the library |

Including `LibraryFacade.h` pulls in every other header of the list.

### ExportFormat

```cpp
enum class ExportFormat {
  PlainTxt2Folder,
  Yolo42Folder,
  PyTorchVisionFolder
};
```

The three values name the three dataset layouts the library is able to write. What each of them puts on the disk is described in the [The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md) subsection.

### ExportContext

The data class a single export run is driven with. Create it with `LibraryFacade::create_export_context()`.

| Field | Meaning |
| --- | --- |
| `std::string export_path` | the destination directory of the export, mandatory |
| `ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr dbProvider` | the annotations database to read the records out of, mandatory |
| `IImageCropperFacilityPtr cropper` | the image cropping service, needed by `ExportFormat::PyTorchVisionFolder` only |

`ExportContextPtr` is the `std::shared_ptr<ExportContext>` alias. An `ImagesAnnotatorDataDrivers011::IAnnotationsDBPtr` may be assigned to `dbProvider` directly, since `IAnnotationsDB` derives from `IImagesPathsDBProvider`.

### IExporter

```cpp
virtual bool export_db(ExportContextPtr ectx) = 0;
```

The single method of an exporter. It writes the database named by the context out in the layout that exporter implements and returns `true` when the run as a whole went through. Records it cannot process - a record without rectangles, an image file that is not there - are skipped and reported through the library log, they do not fail the run. `IExporterPtr` is the `std::shared_ptr<IExporter>` alias.

### IImageCropperFacility

This is the one interface the consuming project implements itself:

```cpp
virtual bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                           std::string& tofpath) = 0;
virtual IImageCropperFacilityPtr clone() = 0;
```

The library decodes no image format of its own, so the one export that has to cut a rectangle out of a picture asks its consumer to do it over whatever imaging stack that project already links.

- `ir` is the record naming the image to read, through its `ImageRecord::get_full_path()`.
- `irr` is the rectangle to cut out, in the image own pixel coordinates (`name`, `x`, `y`, `width`, `height`).
- `tofpath` is an in-out parameter: the library fills it with the destination file path it wants, and an implementation may rewrite it, for example to append the extension of the format it encodes to.
- Return `true` once the cropped image has been written out.

`clone()` has to produce a copy sharing no mutable decoding state with the original. The exporters shipped today only ever call `crop_out_2_fs()`, but the method is a part of the interface and has to be implemented.

`ImageRecordPtr` and `ImageRecordRectPtr` are the `ImagesAnnotatorDataDrivers011` record pointers.

### LibraryContext and ILib

`LibraryContext` drives the single shot entry point of the library. It carries the same in-fields as `ExportContext` plus the wanted format, and receives one out-field back:

| Field | Direction | Meaning |
| --- | --- | --- |
| `ExportFormat format` | in | the layout to write, defaults to `ExportFormat::PlainTxt2Folder` |
| `std::string export_path` | in | the destination directory of the export |
| `IImagesPathsDBProviderPtr dbProvider` | in | the annotations database to read |
| `IImageCropperFacilityPtr cropper` | in | the image cropper, when the format needs one |
| `IExporterPtr exporter` | out | the exporter instance the last `libcall` ran |

`ILib::libcall(LibraryContextPtr ctx)` builds the exporter for `ctx->format`, stores it in `ctx->exporter`, copies the in-fields into a fresh export context and runs the export. It returns `false` when the format is unknown or the export itself failed. Projects that want a finer grained control should rather build the exporter directly with `LibraryFacade::create_exporter()`.

### LibraryFacade

A class of static factory methods only, and the only entry point a consuming project needs:

| Method | Returns |
| --- | --- |
| `create_library_context()` | a new empty `LibraryContextPtr` |
| `create_default_lib()` | the default `ILibPtr` implementation |
| `create_library(LibraryContextPtr ctx)` | the `ILibPtr` implementation appropriate for the given context |
| `create_export_context()` | a new empty `ExportContextPtr` |
| `create_exporter(const ExportFormat& format)` | a new `IExporterPtr` for the format, or a `nullptr` for an unknown one |
| `library_version()` | the version string of the library binary in use |

### A complete example

The program below opens an ImagesAnnotator project file through the data drivers library and writes it out as a YOLO v4 training directory. See the [Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md) section for the CMake side of it.

```cpp
#include <ImagesAnnotatorDataDrivers-0.11/LibraryFacade.h>
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>

#include <filesystem>
#include <iostream>

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

  auto ectx = iade::LibraryFacade::create_export_context();

  ectx->dbProvider = db;
  ectx->export_path = argv[2];

  auto exporter =
      iade::LibraryFacade::create_exporter(iade::ExportFormat::Yolo42Folder);

  if (exporter == nullptr) {
    std::cerr << "no exporter available for the requested format\n";
    return 1;
  }

  if (!exporter->export_db(ectx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported by the exporters library version "
            << iade::LibraryFacade::library_version() << '\n';

  return 0;
}
```

The `std::filesystem::create_directories` call is there because only the YOLO v4 exporter creates its destination directory on its own. The two other formats expect `export_path` to exist already.

### Implementing an image cropper

`ExportFormat::PyTorchVisionFolder` is the only format that needs a cropper. The sketch below wires one up over the imaging routines the consuming project already has - substitute your own decoding and encoding calls for the `my_imaging` ones:

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

Hand the instance over through the export context and the exporter picks it up:

```cpp
ectx->cropper = std::make_shared<MyCropper>();
```

Every rectangle carrying the same annotation name inside one image is offered the very same `tofpath`, so a cropper that has to keep them all apart has to make the path unique itself.

### The IADE_API marker

Every installable class is marked with the `IADE_API` macro of [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h), and the library is compiled with the hidden symbol visibility, so that only the marked interface leaves the shared object. That is a correctness requirement rather than a size optimisation: the data drivers library this one links against is built from the very same project template and carries its own `simple_logger::SimpleLogger` and `project_decls` definitions. Were both sets exported, the dynamic linker would be free to bind one library's calls into the other library's differently laid out classes.

The hiding does not reach the factory. A `std::make_shared` instantiation names its class in its own mangled name and stays weak and exported whatever the visibility is, so the implementation namespace here is `iade0impl` and not the `lib0impl` the project template - and the data drivers library with it - uses.

A working consumer of the whole interface is kept in the tree as the `CTEST_Exporters` component test, [src/lib/libmain/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/lib/libmain/exporters/tests/component/Exporters/CTEST_Exporters.cpp): it links the real shared library and drives it through the public headers only, exactly the way a downstream project does.
