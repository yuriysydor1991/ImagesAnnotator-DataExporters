# Using the library in your project

The library is meant to be consumed by other projects: it produces no executable of its own, only the `libImagesAnnotatorDataExporters-0.13.so` shared object with its installable headers and a CMake package. This section describes what a downstream project has to do to build against it.

## What has to be installed first

Two things have to be installed before a consumer may be configured:

- the [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) library, which defines the annotations database and the image records this library reads - see the [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md) subsection;
- this library itself, built and installed as described in the [Installing](/doc/sections/en_US/7-installing/7-installing.md) section.

The install puts the following into the chosen prefix:

```
<prefix>/include/ImagesAnnotatorDataExporters-0.13/     the public headers
<prefix>/lib/libImagesAnnotatorDataExporters-0.13.so    the shared object, soname .so.0
<prefix>/lib/cmake/ImagesAnnotatorDataExporters-0.13/   the CMake package files
```

## Finding the package with CMake

```cmake
cmake_minimum_required(VERSION 3.13)

project(MyExportingTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImagesAnnotatorDataExporters-0.13 0.13 REQUIRED)

add_executable(my-exporting-tool main.cpp)

target_link_libraries(
  my-exporting-tool
  PRIVATE ImagesAnnotatorDataExporters-0.13::ImagesAnnotatorDataExporters-0.13
)
```

Both the package name and the imported target carry the library major and minor version, so a future release may be installed side by side with this one. The version request is matched against the installed `ImagesAnnotatorDataExporters-0.13ConfigVersion.cmake` file with the `SameMajorVersion` compatibility rule.

The installed package configuration file, generated from [src/lib/cmake/ExportersLibraryConfig.cmake.in](/src/lib/cmake/ExportersLibraryConfig.cmake.in), calls `find_dependency()` on the data drivers package before it reads the exported targets. The data drivers library is linked `PUBLIC` because the installable headers of this library name its record types, so linking the target above brings the data drivers include path and shared object along with it. A separate `find_package()` for the data drivers is not needed in a consumer, although calling one does no harm.

The C++ `17` standard is not optional: the public headers and the records they name are compiled as C++17.

## Pointing CMake at the install prefixes

When either library was installed outside of the system default prefixes, name their prefixes through `CMAKE_PREFIX_PATH` when configuring your project:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH="/opt/iadd;/opt/iade"
```

The very same variable is what this library itself needs at its own configure time to find the data drivers.

## Including the headers

Two include roots are exported, so both spellings compile:

```cpp
#include <ImagesAnnotatorDataExporters-0.13/IADataExportersFacade.h>  // recommended
#include <IADataExportersFacade.h>                                    // also works
```

Prefer the prefixed one. Header names such as `IExporter.h` and `ExportersAPI.h` are generic enough to collide in a busy include path, and the prefix pins the library version a translation unit is talking to.

`IADataExportersFacade.h` includes every other public header of the library, so it is usually the only one a consumer names.

## Aliasing the namespaces

The interface namespaces of both libraries carry their major and minor version numbers. Alias them once and the version bump stays a one line change:

```cpp
namespace iadd = ImagesAnnotatorDataDrivers013;
namespace iade = ImagesAnnotatorDataExporters013;
```

## What your project has to supply

- **The database.** `IADataExportersContext::set_db_provider()` takes an `ImagesAnnotatorDataDrivers013::IImagesPathsDBProviderPtr`. Usually it comes from `iadd::IADataDriversFacade::open_annotations_db("project.json")`, but any implementation of that interface will do - records assembled in memory work just as well.
- **The destination directory.** The YOLO v4, the four darknetxx, the three Ultralytics YOLO, the COCO, the Pascal VOC and the Create ML exports create their own. For the plain text and the PyTorch Vision layouts the `export_path` directory has to exist before the export is started.
- **An image cropper, for one layout only.** `PyTorchExportContext` cuts the annotated rectangles out of the pictures, and the library decodes no image format itself. Implement `IImageCropperFacility` over the imaging stack your project already links and pass the instance in through `PyTorchExportContext::set_cropper()`. Without it that export fails immediately. The [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection carries an implementation sketch. The twelve other layouts need no cropper at all.

## A minimal consumer

The `main.cpp` below opens a project file and writes it out through the single shot `IADataExportersLib::perform_export` entry point:

```cpp
#include <ImagesAnnotatorDataDrivers-0.13/IADataDriversFacade.h>
#include <ImagesAnnotatorDataExporters-0.13/IADataExportersFacade.h>

#include <filesystem>
#include <iostream>
#include <memory>

namespace iadd = ImagesAnnotatorDataDrivers013;
namespace iade = ImagesAnnotatorDataExporters013;

int main()
{
  auto db = iadd::IADataDriversFacade::open_annotations_db("project.json");

  if (db == nullptr) {
    std::cerr << "fail to open the project file\n";
    return 1;
  }

  auto ctx = iade::IADataExportersFacade::create_plain_txt_library_context();

  ctx->set_export_path("plain-dataset");
  ctx->set_db_provider(db);

  std::filesystem::create_directories(ctx->get_export_path());

  auto lib = iade::IADataExportersFacade::create_library(ctx);

  if (lib == nullptr || !lib->perform_export(ctx)) {
    std::cerr << "the export has failed\n";
    return 1;
  }

  std::cout << "exported with the library version "
            << iade::IADataExportersFacade::library_version() << '\n';

  return 0;
}
```

Instantiate `iade::Yolo4ExportContext`, one of the three `iade::Ultralytics*ExportContext` ones, `iade::CocoExportContext`, `iade::PascalVocExportContext`, `iade::CreateMLExportContext`, one of the four darknetxx `iade::Dxxwjz2ExportContext`, `iade::Dxxwjz3ExportContext`, `iade::Dxxwjz2pExportContext` and `iade::Dxxwjz3pExportContext` ones or `iade::PyTorchExportContext` instead to get one of the twelve other layouts, described in the [The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md) subsection. Building the exporter directly with `iade::IADataExportersFacade::create_exporter()` gives the same result with a finer grained control - see the [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection.

## Running the result

Both shared objects have to be reachable by the dynamic linker at the run time. When they were installed into a prefix the system does not search by default, either register that prefix (for example by adding it to `/etc/ld.so.conf.d/` and running `ldconfig`) or name it in the environment:

```
LD_LIBRARY_PATH=/opt/iade/lib:/opt/iadd/lib ./my-exporting-tool
```

`iade::IADataExportersFacade::library_version()` reports the version of the binary that was actually loaded, which is the quickest way to tell which of several installed copies your program ended up with.

## A worked example inside this project

The `CTEST_Exporters` component test, [src/exporters/tests/component/Exporters/CTEST_Exporters.cpp](/src/exporters/tests/component/Exporters/CTEST_Exporters.cpp), links the produced shared library and drives it through the installable headers only, exactly the way a downstream project does - including a small `IImageCropperFacility` implementation. Enable it with the `ENABLE_COMPONENT_TESTS` CMake option described in the [Project build](/doc/sections/en_US/5-project-build/5-project-build.md) section.
