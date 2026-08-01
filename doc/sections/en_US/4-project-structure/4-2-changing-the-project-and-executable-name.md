## Changing the project and the library name

The project name is declared by the `project()` command of the root [CMakeLists.txt](/CMakeLists.txt):

```
project(
  ImagesAnnotatorDataExporters
  VERSION 0.11.0
  ...
)
```

Everything a consumer sees is derived from that name and from the version. [cmake/template-project-misc-variables-declare.cmake](/cmake/template-project-misc-variables-declare.cmake) composes the `PROJECT_LIBRARY_NAME` cache entry as `${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}`, and because `LIB_INCLUDE_MINOR_IN_NAME` is ON by default it appends `.${CMAKE_PROJECT_VERSION_MINOR}` too, which for the current version yields `ImagesAnnotatorDataExporters-0.11` and with it:

- the binary `libImagesAnnotatorDataExporters-0.11.so` (the `lib` prefix is added by CMake, `VERSION` is `0.11.0` and `SOVERSION` is `0`),
- the public header directory `include/ImagesAnnotatorDataExporters-0.11/`,
- the CMake package directory `<libdir>/cmake/ImagesAnnotatorDataExporters-0.11/`,
- the imported target `ImagesAnnotatorDataExporters-0.11::ImagesAnnotatorDataExporters-0.11`.

So changing the `project()` name renames the binary, the include sub-directory, the CMake package and the imported target in one go, and every downstream `find_package(ImagesAnnotatorDataExporters-0.11 0.11 REQUIRED)` call has to be updated with it. The C++ names do not follow automatically: the `ImagesAnnotatorDataExporters011` namespace, the `IADE_API` macro, the `IMAGES_ANNOTATOR_DATA_EXPORTERS_BUILDING` compile definition and the header include guards are spelled out in the sources and would have to be renamed by hand.

Optional name segments (a minor and micro version in the name, or a trailing tag such as `-dev`) are configured without touching any file - see [Customizing the installable library name segments](/doc/sections/en_US/5-project-build/compression/5-23-customizing-library-name-segments.md).

Note that the project builds no executable. The only programs it produces are the test binaries described in [Project tests](/doc/sections/en_US/4-project-structure/4-4-project-tests.md), and their names are fixed in their own `CMakeLists.txt` files rather than derived from the project name.
