**The ImagesAnnotator annotations dataset exporters library**

# What is it

The `ImagesAnnotatorDataExporters` is a C++17 shared library which turns the annotations of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) application - the annotated images with the named rectangles drawn over them - into the training dataset layouts the machine learning frameworks read.

The code used to live inside the application itself. It was extracted into a library of its own so that the ImagesAnnotator application and **any other tool** may produce one and the same dataset out of an annotations project without duplicating the exporters.

Everything a consuming project touches is hidden behind the abstract interfaces of the [src/lib/facade/public](/src/lib/facade/public) headers, so no implementation class, and none of the library third party dependencies, leak into the downstream code.

See more at the [kytok.org.ua](http://www.kytok.org.ua/)

💵 Donate at [http://kytok.org.ua/page/pozertvy](http://kytok.org.ua/page/pozertvy)

# Features

- **Three dataset layouts out of one database** - selected by the `LibraryContext` descendant instantiated and implemented by one exporter class each:
  - `PlainTxtExportLibraryContext` - one `<annotation-name>.txt` file per annotation name, each line naming an image and its rectangles;
  - `Yolo4ExportLibraryContext` - the whole darknet training directory of the YOLO v4 detector: `data/obj.names`, `data/obj.data`, the `cfg/yolov4-obj.cfg` descriptor of the whole 162 layer YOLO v4 network written for the classes of the project, the copied images with their normalised `.txt` label files, the `train.txt` and `val.txt` lists and an empty `backup/`;
  - `PyTorchExportLibraryContext` - the classification layout the PyTorch Vision `ImageFolder` dataset reads: one directory per annotation name holding the images cropped down to the rectangles of that name.
- **A one shot entry point** - fill the `LibraryContext` descendant of the wanted layout with the destination directory and the database, and `ILib::perform_export()` builds the right exporter and runs it. `LibraryFacade::create_exporter()` gives the same result with a finer grained control.
- **Web hosted images are preloaded** - a record pointing at a web page is downloaded through [libcurl](https://curl.se/libcurl/) into a temporary preloads cache before the export touches it, so a project mixing local and remote images exports as one.
- **Robust over a partial database** - a record without rectangles, with a zero size or with an unreadable image file is logged and skipped, the export run itself carries on.
- **No image codec of its own** - the library copies image files as they are. The one format that has to cut rectangles out asks the consuming project to do it through the `IImageCropperFacility` interface, over whatever imaging stack that project already links.
- **A versioned installable interface** - the namespace, the binary, the header directory and the CMake package all carry the `0.11` major and minor pair, so two minor releases install side by side.

# Usage example

The library is consumed through a CMake package. The following program was compiled, linked and run against the installed library:

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

  auto ctx = iade::LibraryFacade::create_yolo4_library_context();

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

The `ImagesAnnotatorDataExporters011` namespace name carries the library major and minor version numbers on purpose: two library versions may coexist inside a single translation unit without any symbol clash. Alias it, as shown above, and the version bump stays a one line change on your side.

Both `#include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>` and a plain `#include <LibraryFacade.h>` work for an installed consumer, since the library exports the include root along with its versioned subdirectory. The prefixed form is the recommended one: header names like `LibraryFacade.h`, `LibraryContext.h` or `ILib.h` are generic enough to collide in a busy include path - the data drivers library installs headers of exactly those names.

More on the API and on the produced datasets is in the [dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md), the [produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md) and the [using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md) documentation sections.

# Dependencies

Both of the following are **mandatory** for the library to build:

| CMake option | Library | Why it is needed |
| --- | --- | --- |
| (always on) | [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) | it defines the annotations database and the image records the exporters read |
| `ENABLE_LIBCURL` | [libcurl](https://curl.se/libcurl/) | it preloads the image records that point at a web URL instead of a local file |

The libcurl is first probed system wide and, when it is not found, is pulled by the CMake `FetchContent` - see the [enabling the libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md) section for the source and tag overriding variables. It is linked **privately**: no public header exposes a curl type.

The data drivers library is different. It has to be installed beforehand, it is resolved with `find_package(ImagesAnnotatorDataDrivers-0.11 REQUIRED CONFIG)` and it is linked **publicly**, because the installable headers of this library name its record types. Point the configure at its install prefix with `-DCMAKE_PREFIX_PATH=<prefix>` when it does not sit in a system default one. The [data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md) section covers it in full, the package name included.

The logging component is compiled straight into the shared library, so no logger implementation has to be supplied by the consumer.

# Build and test

The plain build, against a data drivers install in `$HOME/iadd-install`:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/iadd-install
cmake --build build -j$(nproc)
```

The tests are off by default. To build and run them:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/iadd-install \
  -DENABLE_UNIT_TESTS=ON -DENABLE_COMPONENT_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

With both options on the suite holds 61 test cases. The `ENABLE_UNIT_TESTS` targets are compiled straight from the sources against the gmock stand-ins of [src/tests/mocks](/src/tests/mocks), while the `ENABLE_COMPONENT_TESTS` `CTEST_Exporters` links the real shared library and drives it through the public headers only - exactly the way a downstream project does.

Installing is a usual `sudo cmake --install build`, described in detail in the [installing](/doc/sections/en_US/7-installing/7-installing.md) section.

# Where the code came from

The sources were lifted out of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) application, namely its `src/annotator-business/exporters` component together with the dependency closure it needs - the `ImageLoader`, the `ImageRecordUrlAndPathHelper`, the `TypeHelper` and the `CURLController`. The application is to drop its own copy and to consume this library instead.

The annotations database itself - the project file parser, the serializer and the merging rules - is **not** a part of this library. It lives in the sibling [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) project, which this one reads its records through.

Examine the `doc` directory for possible translations of a current md document:
- `uk_UA` at [doc/README.uk_UA.md](doc/README.uk_UA.md)

# Documentation contents

**Document is under the refinement**

1. [Requirements](/doc/sections/en_US/3-requirements/3-requirements.md)
    1. [Required tools for the GNU/Linux based OS](/doc/sections/en_US/3-requirements/3-1-required-tools-for-the-GNU-Linux-based-OS.md)
    1. [Required tools for the MS Windows based OS](/doc/sections/en_US/3-requirements/3-2-required-tools-for-the-MS-Windows-based-OS.md)
    1. [Optional for the tests](/doc/sections/en_US/3-requirements/3-3-optional-for-the-tests.md)
    1. [Optional for the documentation](/doc/sections/en_US/3-requirements/3-4-optional-for-the-documentation.md)
    1. [Optional for the code formatting](/doc/sections/en_US/3-requirements/3-5-optional-for-the-code-formatting.md)
    1. [Optional for the code analyzer (cppcheck)](/doc/sections/en_US/3-requirements/3-6-optional-for-the-code-analyzer-cppcheck.md)
    1. [Optional for the code analyzer with clang-tidy](/doc/sections/en_US/3-requirements/3-7-optional-for-the-code-analyzer-with-clang-tidy.md)
1. [Project structure](/doc/sections/en_US/4-project-structure/4-project-structure.md)
    1. [Project diagrams](/doc/sections/en_US/4-project-structure/4-0-project-diagrams.md)
    1. [Implement code straight away!](/doc/sections/en_US/4-project-structure/4-1-implement-code-straight-away.md)
    1. [The library's installable include header files](/doc/sections/en_US/4-project-structure/4-8-the-librarys-installable-include-header-files.md)
    1. [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md)
    1. [The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md)
    1. [Version tracking and other project parameters](/doc/sections/en_US/4-project-structure/4-3-version-tracking-and-other-project-parameters.md)
    1. [Project tests](/doc/sections/en_US/4-project-structure/4-4-project-tests.md)
        1. [Google Test](/doc/sections/en_US/4-project-structure/4-4-1-google-test.md)
1. [Project build](/doc/sections/en_US/5-project-build/5-project-build.md)
    1. [IDE build](/doc/sections/en_US/5-project-build/5-1-IDE-build.md)
    1. [Command line build](/doc/sections/en_US/5-project-build/5-2-command-line-build.md)
    1. [Quick build scripts](/doc/sections/en_US/5-project-build/5-38-quick-build-scripts.md)
    1. [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md)
    1. Enabling testing
        1. [Enabling unit testing](/doc/sections/en_US/5-project-build/testing/5-3-1-enabling-unit-testing.md)
        1. [Disabling system GTest probe](/doc/sections/en_US/5-project-build/testing/5-3-2-disabling-system-GTest-probe.md)
    1. [Documentation build](/doc/sections/en_US/5-project-build/documentation/5-4-documentation-build.md)
    1. [Configuring the documentation install support](/doc/sections/en_US/5-project-build/documentation/5-5-configuring-the-documentation-install-support.md)
    1. [Customizing the installable library name segments](/doc/sections/en_US/5-project-build/5-23-customizing-library-name-segments.md)
    1. Code quality & sanitizers
        1. [Enabling and performing code formatting target](/doc/sections/en_US/5-project-build/code-quality/5-6-enabling-and-performing-code-formatting-target.md)
        1. [Enabling the static code analyzer target with cppcheck](/doc/sections/en_US/5-project-build/code-quality/5-7-enabling-the-static-code-analyzer-target-with-cppcheck.md)
        1. [Enabling the static code analyzer with clang-tidy](/doc/sections/en_US/5-project-build/code-quality/5-8-enabling-static-code-analyzer-with-clang-tidy.md)
    1. Containers & CI
        1. [Enabling Jenkins pipeline inside Docker container](/doc/sections/en_US/5-project-build/containers-ci/5-17-enabling-Jenkins-pipeline-inside-Docker-container.md)
    1. Packagers
        1. [Enabling DEB package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-10-enabling-DEB-package-generation-with-cpack.md)
        1. [Enabling FreeBSD pkg package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-20-enabling-FreeBSD-pkg-package-generation-with-cpack.md)
        1. [Enabling WIX MSI package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-21-enabling-WIX-MSI-package-generation-with-cpack.md)
        1. [Enabling RPM package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-22-enabling-RPM-package-generation-with-cpack.md)
    1. Libraries
        1. [Enabling the libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md)
        1. [Enabling the OpenCV image cropper](/doc/sections/en_US/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md)
1. Running the tests
    1. [Run tests by the ctest](/doc/sections/en_US/6-running-the-tests/6-3-1-run-tests-by-the-ctest.md)
    1. [Manual tests run](/doc/sections/en_US/6-running-the-tests/6-3-2-manual-tests-run.md)
1. [Installing](/doc/sections/en_US/7-installing/7-installing.md)
1. [Using the library in your project](/doc/sections/en_US/8-using-the-library-in-your-project/8-using-the-library-in-your-project.md)
