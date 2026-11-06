## Version tracking and other project parameters

The library name, the project version, the git commit the build was configured from and the configure timestamp are forwarded into the sources through [src/project-global-decls.h.in](/src/project-global-decls.h.in). [src/CMakeLists.txt](/src/CMakeLists.txt) configures it into the build directory as `project-global-decls.h`, which declares four constants in the `project_decls` namespace:

| Constant | Filled from |
|---|---|
| `PROJECT_NAME` | `IADE_LIBRARY_NAME`, i.e. `ImagesAnnotatorDataExporters-0.15` |
| `PROJECT_BUILD_VERSION` | `PROJECT_VERSION` of the root `project()` command - `0.15.0` |
| `PROJECT_BUILD_COMMIT` | `git rev-parse HEAD`, read by [cmake/enablers/imagesannotator-dataexporters-git-enabler.cmake](/cmake/enablers/imagesannotator-dataexporters-git-enabler.cmake) |
| `PROJECT_CONFIGURE_DATE` | the configure time `string(TIMESTAMP ...)` value |

The generated header is compiled into the library but is not installed. Two places use it: `IADataExportersFacade::library_version()` returns `project_decls::PROJECT_BUILD_VERSION`, which lets a consumer report the version of the shared object it actually loaded, and `helpers::ImageLoader` builds the name of its preload cache directory from the project name and version.

Other parameters declared in [cmake/imagesannotator-dataexporters-misc-variables-declare.cmake](/cmake/imagesannotator-dataexporters-misc-variables-declare.cmake) and worth knowing about:

| Cache variable | Default | Meaning |
|---|---|---|
| `IADE_MAX_LOG_LEVEL` | `3` | the highest severity compiled into the library (`0` error ... `5` trace) |
| `IADE_DEFAULT_LOG_FILE_PATH` | empty | a log file the library writes to besides the standard output |
| `IADE_ENABLE_LOGS_MICROSECONDS_TIME` | `ON` | microseconds in the log message timestamps |
| `IADE_MAINTAINER`, `IADE_MAINTAINER_EMAIL` | placeholders | used by the package generators only |
| `IADE_LIB_INCLUDE_MINOR_IN_NAME`, `IADE_LIB_INCLUDE_MICRO_IN_NAME`, `IADE_LIB_NAME_SUFFIX` | `OFF`, `OFF`, empty | the installable library name segments |

All of them are ordinary cache entries, so they are set on the configure command line, for example `cmake -S . -B build -DIADE_MAX_LOG_LEVEL=5`.
