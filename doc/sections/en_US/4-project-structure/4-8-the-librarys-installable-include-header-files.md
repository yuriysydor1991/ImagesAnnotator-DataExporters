## The library's installable include header files

The [src/lib/facade/public](/src/lib/facade/public) directory holds the whole installable interface of the library - nineteen headers, all in the `ImagesAnnotatorDataExporters013` namespace. Six of them are the API proper and sit at the root of the directory:

| Header | Declares |
|---|---|
| [IADataExportersFacade.h](/src/lib/facade/public/IADataExportersFacade.h) | the `IADataExportersFacade` static methods, the entry point of the library |
| [IADataExportersLib.h](/src/lib/facade/public/IADataExportersLib.h) | `IADataExportersLib::perform_export()` - runs an export described by an `IADataExportersContext` |
| [IADataExportersContext.h](/src/lib/facade/public/IADataExportersContext.h) | the data accessors of a `perform_export()` and of an `export_db()`, plus the `get_exporter()` out-one |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | `IExporter::export_db()` - a single exporter used on its own |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | the interface the consuming project implements to crop images out |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | the `IADE_API` visibility macro |

The thirteen remaining ones are the layout specific `IADataExportersContext` descendants - one per dataset layout the library writes - and they sit together in the [contexts](/src/lib/facade/public/contexts) subdirectory instead of beside the six above, since there are more of them than of the rest of the interface put together and each differs from its neighbours only by the layout it names:

| Header | Declares |
|---|---|
| [contexts/PlainTxtExportContext.h](/src/lib/facade/public/contexts/PlainTxtExportContext.h) | the `IADataExportersContext` of the plain text dataset layout |
| [contexts/Yolo4ExportContext.h](/src/lib/facade/public/contexts/Yolo4ExportContext.h) | the `IADataExportersContext` of the YOLO v4 (darknet) dataset layout |
| [contexts/UltralyticsDetectExportContext.h](/src/lib/facade/public/contexts/UltralyticsDetectExportContext.h) | the `IADataExportersContext` of the Ultralytics YOLO detection dataset layout |
| [contexts/UltralyticsObbExportContext.h](/src/lib/facade/public/contexts/UltralyticsObbExportContext.h) | the `IADataExportersContext` of the Ultralytics YOLO oriented bounding box dataset layout |
| [contexts/UltralyticsSegmentExportContext.h](/src/lib/facade/public/contexts/UltralyticsSegmentExportContext.h) | the `IADataExportersContext` of the Ultralytics YOLO segmentation dataset layout |
| [contexts/CocoExportContext.h](/src/lib/facade/public/contexts/CocoExportContext.h) | the `IADataExportersContext` of the COCO object detection dataset layout |
| [contexts/PascalVocExportContext.h](/src/lib/facade/public/contexts/PascalVocExportContext.h) | the `IADataExportersContext` of the Pascal VOC dataset layout |
| [contexts/CreateMLExportContext.h](/src/lib/facade/public/contexts/CreateMLExportContext.h) | the `IADataExportersContext` of the Create ML object detection dataset layout |
| [contexts/Dxxwjz2ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2ExportContext.h) | the `IADataExportersContext` of the darknetxx training directory layout, the one with the dxxwjz2 network to train |
| [contexts/Dxxwjz3ExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3ExportContext.h) | the `IADataExportersContext` of the darknetxx training directory layout, the one with the dxxwjz3 network to train |
| [contexts/Dxxwjz2pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz2pExportContext.h) | the `IADataExportersContext` of the darknetxx dxxwjz2p training project layout |
| [contexts/Dxxwjz3pExportContext.h](/src/lib/facade/public/contexts/Dxxwjz3pExportContext.h) | the `IADataExportersContext` of the darknetxx dxxwjz3p training project layout |
| [contexts/PyTorchExportContext.h](/src/lib/facade/public/contexts/PyTorchExportContext.h) | the `IADataExportersContext` of the PyTorch Vision dataset layout, plus its image cropper |

Each of the thirteen reaches the six above through a `../` include, which is what keeps them resolving once installed: the include root of a consumer is the directory holding `ImagesAnnotatorDataExporters-0.13/`, so a plain `#include "IADataExportersContext.h"` from within `contexts/` would look for it at that root and miss. None of that concerns a consumer, which reaches all thirteen through `IADataExportersFacade.h` - it includes them - and only a project spelling a layout header out directly names the `contexts/` component itself.

[src/lib/facade/CMakeLists.txt](/src/lib/facade/CMakeLists.txt) installs the directory as a whole under `include/${PROJECT_LIBRARY_NAME}`, which for the current name and version is `include/ImagesAnnotatorDataExporters-0.13/`. Both that sub-directory and the plain include root are exported by the library target, so a consumer may write either form:

```cpp
#include <ImagesAnnotatorDataExporters-0.13/IADataExportersFacade.h>  // recommended
#include <IADataExportersFacade.h>                                    // also works
```

The prefixed form is the safe one - names such as `IExporter.h` are generic enough to collide in a busy include path.

### Why only these headers are visible

The library is built with `CXX_VISIBILITY_PRESET hidden`, so only the entities marked with `IADE_API` leave the shared object. That is a correctness requirement rather than a size optimisation. The ImagesAnnotatorDataDrivers library this one links against comes from the same project template and exports its own `default_logger::DefaultLogger` and `project_decls` symbols. Were both sets exported, the dynamic linker would bind one library's calls to the other library's definitions.

Visibility alone leaves one hole. A `std::make_shared` instantiation names its class in its own mangled name and stays weak and exported whatever the visibility is, so the implementation namespace here is `iade0impl` and not the `lib0impl` the project template - and the data drivers library with it - uses. The two `LibFactory` classes do not even share a vtable layout, and before the rename the linker did bind one library's `std::make_shared<lib0impl::LibFactory>()` to the other's definition.

So a new public class belongs in [src/lib/facade/public](/src/lib/facade/public) - a new dataset layout context in its [contexts](/src/lib/facade/public/contexts) subdirectory, reaching the headers above it through `../` - and has to be marked with `IADE_API`; every other component under [src](/src) stays private to the shared object and is reached through the abstract interfaces above.

### The installed CMake package

Along with the binary and the headers the build installs a CMake package, generated by [src/lib/cmake/lib-cmake-module-gen.cmake](/src/lib/cmake/lib-cmake-module-gen.cmake) from [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in). It lands in `<libdir>/cmake/ImagesAnnotatorDataExporters-0.13/` and consists of three files: the exported targets, an `ImagesAnnotatorDataExporters-0.13ConfigVersion.cmake` written by `write_basic_package_version_file()` with `SameMajorVersion` compatibility, and the `ImagesAnnotatorDataExporters-0.13Config.cmake` that `find_dependency()`s the data drivers package before including the targets - the public headers name its record types, so it has to be resolved first.

A downstream project therefore needs no more than:

```cmake
find_package(ImagesAnnotatorDataExporters-0.13 0.13 REQUIRED)

target_link_libraries(
  your_target
  PRIVATE ImagesAnnotatorDataExporters-0.13::ImagesAnnotatorDataExporters-0.13
)
```

See [Installing](/doc/sections/en_US/7-installing/7-installing.md) and [Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).
