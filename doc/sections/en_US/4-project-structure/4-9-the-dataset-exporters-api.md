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
| [IExporter.h](/src/lib/facade/public/IExporter.h) | the `IExporter` abstract exporter interface |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | the `IImageCropperFacility` interface the consuming project implements |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | the `LibraryContext` in and out data class both entry points are driven with |
| [PlainTxtExportLibraryContext.h](/src/lib/facade/public/PlainTxtExportLibraryContext.h) | the `LibraryContext` descendant of the plain text dataset layout |
| [Yolo4ExportLibraryContext.h](/src/lib/facade/public/Yolo4ExportLibraryContext.h) | the `LibraryContext` descendant of the YOLO v4 (darknet) dataset layout |
| [UltralyticsDetectExportLibraryContext.h](/src/lib/facade/public/UltralyticsDetectExportLibraryContext.h) | the `LibraryContext` descendant of the Ultralytics YOLO detection dataset layout |
| [UltralyticsObbExportLibraryContext.h](/src/lib/facade/public/UltralyticsObbExportLibraryContext.h) | the `LibraryContext` descendant of the Ultralytics YOLO oriented bounding box dataset layout |
| [UltralyticsSegmentExportLibraryContext.h](/src/lib/facade/public/UltralyticsSegmentExportLibraryContext.h) | the `LibraryContext` descendant of the Ultralytics YOLO segmentation dataset layout |
| [CocoExportLibraryContext.h](/src/lib/facade/public/CocoExportLibraryContext.h) | the `LibraryContext` descendant of the COCO object detection dataset layout |
| [PascalVocExportLibraryContext.h](/src/lib/facade/public/PascalVocExportLibraryContext.h) | the `LibraryContext` descendant of the Pascal VOC dataset layout |
| [PyTorchExportLibraryContext.h](/src/lib/facade/public/PyTorchExportLibraryContext.h) | the `LibraryContext` descendant of the PyTorch Vision dataset layout, with the image cropper of that layout |
| [ILib.h](/src/lib/facade/public/ILib.h) | the `ILib` abstract library interface with its `perform_export` method |
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | the `LibraryFacade` factory class, the entry point of the library |

Including `LibraryFacade.h` pulls in every other header of the list.

### The dataset layouts

```cpp
class PlainTxtExportLibraryContext : public LibraryContext;
class Yolo4ExportLibraryContext : public LibraryContext;
class UltralyticsDetectExportLibraryContext : public LibraryContext;
class UltralyticsObbExportLibraryContext : public LibraryContext;
class UltralyticsSegmentExportLibraryContext : public LibraryContext;
class CocoExportLibraryContext : public LibraryContext;
class PascalVocExportLibraryContext : public LibraryContext;
class CreateMLExportLibraryContext : public LibraryContext;
class PyTorchExportLibraryContext : public LibraryContext;
```

The nine `LibraryContext` descendants name the nine dataset layouts the library is able to write. Instantiating one is what picks the layout, and the library maps that type onto the exporter which writes it. All but the last add nothing to `LibraryContext`; `PyTorchExportLibraryContext` adds the `get_cropper()` / `set_cropper()` pair, since cutting pixels out is what its layout alone does - it has a subsection of its own below. What each of them puts on the disk is described in the [The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md) subsection.

Four of them are of the YOLO family, and they are not interchangeable. `Yolo4ExportLibraryContext` writes the darknet training directory, which names its classes, lists its images and carries its whole network in files of its own. The three `Ultralytics*` ones write the layout YOLO v5 introduced and every Ultralytics release since - v8, v11 and the ones after them - reads: a single `data.yaml` descriptor over the `images/train` and `labels/train` directories. Those three share every byte of that layout and differ only in the label file line of a rectangle, which is what the trained task - detection, oriented boxes or segmentation - expects.

`CocoExportLibraryContext` and `PascalVocExportLibraryContext` are the two layouts of the list read by something other than a YOLO training run: Detectron2, MMDetection, torchvision, the HuggingFace detection transformers, CVAT, FiftyOne, Label Studio and Roboflow all take the COCO JSON descriptor, and the VOC devkit directory is what torchvision `VOCDetection`, the MMDetection `XMLDataset` and LabelImg read. Both also hand a rectangle over untouched, where every YOLO layout divides its numbers by the image size first - the COCO `bbox` as the `[x, y, width, height]` pixels of the image, the VOC `bndbox` as the two corner points in those same pixels. `PascalVocExportLibraryContext` is on top of that one of the two layouts which write an annotation name instead of its position in the sorted set of names, so a name added to a project renumbers nothing that was exported before it.

`CreateMLExportLibraryContext` is the one layout of the list which leads off the training frameworks entirely: it writes what Apple's `MLObjectDetector` reads, and a model trained from it is a Core ML detector that runs on an iPhone. It is also the only layout whose coordinates are neither a corner nor normalised - `x`/`y` is the **centre** of the box in the image own pixels, which is the halving the three `Ultralytics*` layouts perform before they divide by the image size - and the second, next to `PascalVocExportLibraryContext`, to write a class name instead of an index.

### LibraryContext

The single data class of the library, the one both of its entry points are driven with: the one shot `ILib::perform_export()` and the `IExporter::export_db()` of an exporter built by hand. Create it with the `LibraryFacade` factory method of the wanted layout - `create_plain_txt_library_context()`, `create_yolo4_library_context()`, `create_ultralytics_detect_library_context()`, `create_ultralytics_obb_library_context()`, `create_ultralytics_segment_library_context()`, `create_coco_library_context()`, `create_pascal_voc_library_context()`, `create_createml_library_context()` or `create_pytorch_library_context()` - or by instantiating that descendant yourself, which is what a consumer templated over the layout type does.

The data it carries is private and reached through accessors only. Every getter hands out a `const` reference to what the context holds, every setter copies the given value in:

| Accessors | Direction | Meaning |
| --- | --- | --- |
| `get_export_path()`, `set_export_path()` | in | the destination directory of the export, a `std::string`, mandatory |
| `get_db_provider()`, `set_db_provider()` | in | the annotations database to read the records out of, an `ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr`, mandatory |
| `get_exporter()`, `set_exporter()` | out | the exporter instance the last `perform_export` ran, an `IExporterPtr` |

`LibraryContextPtr` is the `std::shared_ptr<LibraryContext>` alias. An `ImagesAnnotatorDataDrivers011::IAnnotationsDBPtr` may be handed to `set_db_provider()` directly, since `IAnnotationsDB` derives from `IImagesPathsDBProvider`.

### PyTorchExportLibraryContext

The one descendant which carries data of its own, because the PyTorch Vision layout is the one which cuts rectangles out of the pictures:

| Accessors | Direction | Meaning |
| --- | --- | --- |
| `get_cropper()`, `set_cropper()` | in | the image cropping service, an `IImageCropperFacilityPtr`. Optional in a library built with OpenCV, mandatory in one built without it |

No other layout carries a cropper slot it would never read. An `export_db()` of the PyTorch Vision exporter driven with any other context therefore finds no consumer cropper and falls back to the library's own, or fails when this build ships none.

### IExporter

```cpp
virtual bool export_db(LibraryContextPtr ectx) = 0;
```

The single method of an exporter. It writes the database named by the context out in the layout that exporter implements and returns `true` when the run as a whole went through. Records it cannot process - a record without rectangles, an image file that is not there - are skipped and reported through the library log, they do not fail the run.

The layout the context type names is not looked at here: the layout written is the one of the exporter itself, so even the `LibraryContext` base class, which names no layout of its own, drives an `export_db()` just fine. An implementation must not keep the context beyond the call, since a context holding that exporter back would close a pointer cycle. `IExporterPtr` is the `std::shared_ptr<IExporter>` alias.

### IImageCropperFacility

This is the one interface a consuming project may have to implement itself:

```cpp
virtual bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                           std::string& tofpath) = 0;
virtual IImageCropperFacilityPtr clone() = 0;
```

The library decodes no image format of its own, so the one export that has to cut a rectangle out of a picture asks its consumer to do it over whatever imaging stack that project already links.

A library built with OpenCV ships an implementation of its own, so this interface only has to be implemented by a project that wants its own cropping - or by one consuming a library built without OpenCV. See [Enabling the OpenCV image cropper](/doc/sections/en_US/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md).

- `ir` is the record naming the image to read, through its `ImageRecord::get_full_path()`.
- `irr` is the rectangle to cut out, in the image own pixel coordinates (`name`, `x`, `y`, `width`, `height`).
- `tofpath` is an in-out parameter: the library fills it with the destination file path it wants, and an implementation may rewrite it, for example to append the extension of the format it encodes to.
- Return `true` once the cropped image has been written out.

`clone()` has to produce a copy sharing no mutable decoding state with the original. The exporters shipped today only ever call `crop_out_2_fs()`, but the method is a part of the interface and has to be implemented.

`ImageRecordPtr` and `ImageRecordRectPtr` are the `ImagesAnnotatorDataDrivers011` record pointers.

### ILib

`ILib::perform_export(LibraryContextPtr ctx)` is the single shot entry point of the library. It builds the exporter of the layout the context names, publishes it through `ctx->set_exporter()` and runs the export over that very same context. It returns `false` when the context names no known layout or the export itself failed. Projects that want a finer grained control should rather build the exporter directly with `LibraryFacade::create_exporter()` and call `export_db()` on it - with the same context in both hands.

### LibraryFacade

A class of static factory methods only, and the only entry point a consuming project needs:

| Method | Returns |
| --- | --- |
| `create_plain_txt_library_context()` | a new empty `PlainTxtExportLibraryContextPtr` |
| `create_yolo4_library_context()` | a new empty `Yolo4ExportLibraryContextPtr` |
| `create_ultralytics_detect_library_context()` | a new empty `UltralyticsDetectExportLibraryContextPtr` |
| `create_ultralytics_obb_library_context()` | a new empty `UltralyticsObbExportLibraryContextPtr` |
| `create_ultralytics_segment_library_context()` | a new empty `UltralyticsSegmentExportLibraryContextPtr` |
| `create_coco_library_context()` | a new empty `CocoExportLibraryContextPtr` |
| `create_pascal_voc_library_context()` | a new empty `PascalVocExportLibraryContextPtr` |
| `create_pytorch_library_context()` | a new empty `PyTorchExportLibraryContextPtr`, the one carrying the cropper |
| `create_default_lib()` | the default `ILibPtr` implementation |
| `create_library(LibraryContextPtr ctx)` | the `ILibPtr` implementation appropriate for the given context |
| `create_exporter(const LibraryContextPtr& ctx)` | a new `IExporterPtr` for the layout of the context, or a `nullptr` for a context naming no known layout |
| `create_image_cropper()` | the cropper the library ships itself, or a `nullptr` in a build without OpenCV |
| `library_version()` | the version string of the library binary in use |

### A complete example

The program below opens an ImagesAnnotator project file through the data drivers library and writes it out as a YOLO v4 training directory. See the [Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md) section for the CMake side of it.

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

The `std::filesystem::create_directories` call is there because only the YOLO v4, the three Ultralytics YOLO, the COCO and the Pascal VOC exporters create their destination directory on their own. The plain text and the PyTorch Vision formats expect `export_path` to exist already.

### Implementing an image cropper

`PyTorchExportLibraryContext` is the only layout that needs a cropper. The sketch below wires one up over the imaging routines the consuming project already has - substitute your own decoding and encoding calls for the `my_imaging` ones:

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

Hand the instance over through the `PyTorchExportLibraryContext` and the exporter picks it up:

```cpp
auto ctx = iade::LibraryFacade::create_pytorch_library_context();

ctx->set_cropper(std::make_shared<MyCropper>());
```

Every rectangle carrying the same annotation name inside one image is offered the very same `tofpath`, so a cropper that has to keep them all apart has to make the path unique itself.

### The IADE_API marker

Every installable class is marked with the `IADE_API` macro of [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h), and the library is compiled with the hidden symbol visibility, so that only the marked interface leaves the shared object. That is a correctness requirement rather than a size optimisation: the data drivers library this one links against is built from the very same project template and carries its own `default_logger::DefaultLogger` and `project_decls` definitions. Were both sets exported, the dynamic linker would be free to bind one library's calls into the other library's differently laid out classes.

The hiding does not reach the factory. A `std::make_shared` instantiation names its class in its own mangled name and stays weak and exported whatever the visibility is, so the implementation namespace here is `iade0impl` and not the `lib0impl` the project template - and the data drivers library with it - uses.

A working consumer of the whole interface is kept in the tree as the `CTEST_Exporters` component test, [src/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/exporters/tests/component/Exporters/CTEST_Exporters.cpp): it links the real shared library and drives it through the public headers only, exactly the way a downstream project does.
