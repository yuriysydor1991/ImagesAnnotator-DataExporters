## The library's installable include header files

The [src/lib/facade/public](/src/lib/facade/public) directory holds the whole installable interface of the library - fifteen headers, all in the `ImagesAnnotatorDataExporters011` namespace. Six of them are the API proper and sit at the root of the directory:

| Header | Declares |
|---|---|
| [LibraryFacade.h](/src/lib/facade/public/LibraryFacade.h) | the `LibraryFacade` static methods, the entry point of the library |
| [ILib.h](/src/lib/facade/public/ILib.h) | `ILib::perform_export()` - runs an export described by a `LibraryContext` |
| [LibraryContext.h](/src/lib/facade/public/LibraryContext.h) | the data accessors of a `perform_export()` and of an `export_db()`, plus the `get_exporter()` out-one |
| [IExporter.h](/src/lib/facade/public/IExporter.h) | `IExporter::export_db()` - a single exporter used on its own |
| [IImageCropperFacility.h](/src/lib/facade/public/IImageCropperFacility.h) | the interface the consuming project implements to crop images out |
| [ExportersAPI.h](/src/lib/facade/public/ExportersAPI.h) | the `IADE_API` visibility macro |

The nine remaining ones are the layout specific `LibraryContext` descendants - one per dataset layout the library writes - and they sit together in the [contexts](/src/lib/facade/public/contexts) subdirectory instead of beside the six above, since there are more of them than of the rest of the interface put together and each differs from its neighbours only by the layout it names:

| Header | Declares |
|---|---|
| [contexts/PlainTxtExportLibraryContext.h](/src/lib/facade/public/contexts/PlainTxtExportLibraryContext.h) | the `LibraryContext` of the plain text dataset layout |
| [contexts/Yolo4ExportLibraryContext.h](/src/lib/facade/public/contexts/Yolo4ExportLibraryContext.h) | the `LibraryContext` of the YOLO v4 (darknet) dataset layout |
| [contexts/UltralyticsDetectExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsDetectExportLibraryContext.h) | the `LibraryContext` of the Ultralytics YOLO detection dataset layout |
| [contexts/UltralyticsObbExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsObbExportLibraryContext.h) | the `LibraryContext` of the Ultralytics YOLO oriented bounding box dataset layout |
| [contexts/UltralyticsSegmentExportLibraryContext.h](/src/lib/facade/public/contexts/UltralyticsSegmentExportLibraryContext.h) | the `LibraryContext` of the Ultralytics YOLO segmentation dataset layout |
| [contexts/CocoExportLibraryContext.h](/src/lib/facade/public/contexts/CocoExportLibraryContext.h) | the `LibraryContext` of the COCO object detection dataset layout |
| [contexts/PascalVocExportLibraryContext.h](/src/lib/facade/public/contexts/PascalVocExportLibraryContext.h) | the `LibraryContext` of the Pascal VOC dataset layout |
| [contexts/CreateMLExportLibraryContext.h](/src/lib/facade/public/contexts/CreateMLExportLibraryContext.h) | the `LibraryContext` of the Create ML object detection dataset layout |
| [contexts/PyTorchExportLibraryContext.h](/src/lib/facade/public/contexts/PyTorchExportLibraryContext.h) | the `LibraryContext` of the PyTorch Vision dataset layout, plus its image cropper |

Each of the nine reaches the six above through a `../` include, which is what keeps them resolving once installed: the include root of a consumer is the directory holding `ImagesAnnotatorDataExporters-0.11/`, so a plain `#include "LibraryContext.h"` from within `contexts/` would look for it at that root and miss. None of that concerns a consumer, which reaches all nine through `LibraryFacade.h` - it includes them - and only a project spelling a layout header out directly names the `contexts/` component itself.

[src/lib/facade/CMakeLists.txt](/src/lib/facade/CMakeLists.txt) installs the directory as a whole under `include/${PROJECT_LIBRARY_NAME}`, which for the current name and version is `include/ImagesAnnotatorDataExporters-0.11/`. Both that sub-directory and the plain include root are exported by the library target, so a consumer may write either form:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>  // recommended
#include <LibraryFacade.h>                                 // also works
```

The prefixed form is the safe one - names such as `IExporter.h` are generic enough to collide in a busy include path.

### Why only these headers are visible

The library is built with `CXX_VISIBILITY_PRESET hidden`, so only the entities marked with `IADE_API` leave the shared object. That is a correctness requirement rather than a size optimisation. The ImagesAnnotatorDataDrivers library this one links against comes from the same project template and exports its own `default_logger::DefaultLogger` and `project_decls` symbols. Were both sets exported, the dynamic linker would bind one library's calls to the other library's definitions.

Visibility alone leaves one hole. A `std::make_shared` instantiation names its class in its own mangled name and stays weak and exported whatever the visibility is, so the implementation namespace here is `iade0impl` and not the `lib0impl` the project template - and the data drivers library with it - uses. The two `LibFactory` classes do not even share a vtable layout, and before the rename the linker did bind one library's `std::make_shared<lib0impl::LibFactory>()` to the other's definition.

So a new public class belongs in [src/lib/facade/public](/src/lib/facade/public) - a new dataset layout context in its [contexts](/src/lib/facade/public/contexts) subdirectory, reaching the headers above it through `../` - and has to be marked with `IADE_API`; every other component under [src](/src) stays private to the shared object and is reached through the abstract interfaces above.

### The installed CMake package

Along with the binary and the headers the build installs a CMake package, generated by [src/lib/cmake/lib-cmake-module-gen.cmake](/src/lib/cmake/lib-cmake-module-gen.cmake) from [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in). It lands in `<libdir>/cmake/ImagesAnnotatorDataExporters-0.11/` and consists of three files: the exported targets, an `ImagesAnnotatorDataExporters-0.11ConfigVersion.cmake` written by `write_basic_package_version_file()` with `SameMajorVersion` compatibility, and the `ImagesAnnotatorDataExporters-0.11Config.cmake` that `find_dependency()`s the data drivers package before including the targets - the public headers name its record types, so it has to be resolved first.

A downstream project therefore needs no more than:

```cmake
find_package(ImagesAnnotatorDataExporters-0.11 0.11 REQUIRED)

target_link_libraries(
  your_target
  PRIVATE ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11
)
```

See [Installing](/doc/sections/en_US/7-installing/7-installing.md) and [Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md).
