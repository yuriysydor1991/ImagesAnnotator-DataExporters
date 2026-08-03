# Project structure

The repository builds a single shared library - `libImagesAnnotatorDataExporters-0.11.so` - that turns an ImagesAnnotator annotations database into a training dataset on disk. There is no executable: apart from the library itself the build only produces test binaries.

The three exporters were moved out of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) application (its `src/annotator-business/exporters` component), so that the application and any other tool may produce the same datasets without duplicating the code. The annotation records themselves are not defined here: they are read through the [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) library, which had earlier taken the annotations database out of that same application.

The top level layout:

| Path | Contents |
|---|---|
| [src/lib/facade/public](/src/lib/facade/public) | the installable public headers - the whole `ImagesAnnotatorDataExporters011` interface |
| [src/lib/facade/LibraryFacade.cpp](/src/lib/facade/LibraryFacade.cpp) | the facade implementation, the entry point of the library |
| [src/lib/libmain](/src/lib/libmain) | `LibMain.cpp` and `LibFactory.cpp` - the implementation core |
| [src/lib/libmain/exporters](/src/lib/libmain/exporters) | the three `IExporter` implementations and `ExportersAliases.h` |
| [src/lib/libmain/helpers](/src/lib/libmain/helpers) | `IHelper.h`, `TypeHelper.h`, `ImageRecordUrlAndPathHelper`, `ImageLoader` |
| [src/lib/libmain/CURL](/src/lib/libmain/CURL) | `CURLController` - the libcurl wrapper used to preload web hosted images |
| [src/lib/cmake](/src/lib/cmake) | generation of the installable CMake package of the library |
| [src/log](/src/log) | the logging macros and the simple logger compiled into the library |
| [cmake](/cmake) | the build system: compile options and the dependency enablers |
| [doc](/doc) | this documentation and the project diagrams |
| [misc](/misc) | the Jenkins pipeline and the Docker files |

The implementation lives in the `iade0impl`, `iannotator::exporters`, `iannotator::exporters::helpers` and `iannotator::exporters::curli` namespaces. None of them is installed - a consuming project only ever sees `ImagesAnnotatorDataExporters011` (an `iade` alias is recommended).

1. [Project diagrams](/doc/sections/en_US/4-project-structure/4-0-project-diagrams.md)
1. [Where the exporters implementation lives](/doc/sections/en_US/4-project-structure/4-1-implement-code-straight-away.md)
1. [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md)
1. [The produced dataset layouts](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md)
1. [The library's installable include header files](/doc/sections/en_US/4-project-structure/4-8-the-librarys-installable-include-header-files.md)
1. [Version tracking and other project parameters](/doc/sections/en_US/4-project-structure/4-3-version-tracking-and-other-project-parameters.md)
1. [Project tests](/doc/sections/en_US/4-project-structure/4-4-project-tests.md)
    1. [Google Test](/doc/sections/en_US/4-project-structure/4-4-1-google-test.md)
