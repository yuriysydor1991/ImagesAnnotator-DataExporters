### Using the produced library

The shared library cannot be started. It becomes useful once another program
links it and calls it through the installable public headers of the
[src/lib/facade/public](/src/lib/facade/public) directory.

After the library is installed, point CMake at it and link the imported target:

```cmake
find_package(ImagesAnnotatorDataExporters-0.11 0.11 REQUIRED)

target_link_libraries(
  your-target
  PRIVATE ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11
)
```

A minimal export then looks like this. The whole run is a single
`ILib::libcall()` call: it picks the exporter for the requested format, hands it
the export path together with the annotations database provider of the
[ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git)
library, and writes the dataset out:

```cpp
#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>

namespace iade = ImagesAnnotatorDataExporters011;

auto ctx = iade::LibraryFacade::create_library_context();

ctx->format = iade::ExportFormat::Yolo42Folder;
ctx->export_path = "/path/to/an/existing/output/directory";
ctx->dbProvider = your_annotations_db;

auto lib = iade::LibraryFacade::create_default_lib();

const bool exported = lib->libcall(ctx);
```

The `PyTorchVisionFolder` format additionally needs an
`IImageCropperFacility` implementation in `ctx->cropper`, because the library
decodes no images itself.

The complete walk-through, including the exporter interfaces used directly, is
in
[Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md),
the interfaces themselves are described in
[The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md)
and the resulting file trees in
[The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md).

A working consumer of exactly this kind lives in the repository itself: the
[CTEST_Exporters](/src/lib/libmain/exporters/tests/component/Exporters/CTEST_Exporters.cpp)
component test links the produced shared library and drives it through the
public headers only.

If the library was installed into a prefix your system loader does not search,
a program that links it needs help finding it at start-up (GNU/Linux based):

```
LD_LIBRARY_PATH=/your/prefix/lib ./your-program
```
