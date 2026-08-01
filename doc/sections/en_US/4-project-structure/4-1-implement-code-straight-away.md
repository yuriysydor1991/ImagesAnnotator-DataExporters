## Where the exporters implementation lives

A call chain through the library is short. A consumer fills a `LibraryContext`, hands it to `ILib::libcall()` and gets the dataset written to `LibraryContext::export_path`:

- [src/lib/facade/LibraryFacade.cpp](/src/lib/facade/LibraryFacade.cpp) - the installed facade. Every static method simply asks `iade0impl::LibFactory` for the requested object, so that no implementation type appears in a public header.
- [src/lib/libmain/LibMain.cpp](/src/lib/libmain/LibMain.cpp) - the only `ILib` implementation. `LibMain::libcall()` creates the exporter for `ctx->format`, stores it back into `ctx->exporter`, copies `export_path`, `dbProvider` and `cropper` into a fresh `ExportContext` and calls `IExporter::export_db()` on it.
- [src/lib/libmain/LibFactory.cpp](/src/lib/libmain/LibFactory.cpp) - the factory. `LibFactory::create_exporter()` maps an `ExportFormat` value onto a concrete exporter and returns a `nullptr` for an unknown one.
- [src/lib/libmain/exporters](/src/lib/libmain/exporters) - `PlainTxt2FolderExporter`, `Yolo42FolderExporter` and `PyTorchVisionFolderExporter`, all in the `iannotator::exporters` namespace. `ExportersAliases.h` pulls the installable interface names and the data drivers record names into that namespace, so the moved application code keeps referring to them unqualified.
- [src/lib/libmain/helpers](/src/lib/libmain/helpers) - shared by all three exporters: `ImageRecordUrlAndPathHelper` decides whether a record points at a local file or at a URL, `ImageLoader` preloads a web hosted record into a local cache directory, and `TypeHelper` shortens the numeric conversions the coordinate maths needs.
- [src/lib/libmain/CURL](/src/lib/libmain/CURL) - `CURLController`, the libcurl wrapper `ImageLoader` downloads through.

The helper and CURL namespaces are nested under `iannotator::exporters` on purpose. The data drivers library carries top level namespaces of the same names, and both libraries end up in the same process.

### Adding a new export format

1. Add an enumerator to `ExportFormat` in [src/lib/facade/public/ExportFormat.h](/src/lib/facade/public/ExportFormat.h). This is the only installed header the change touches.
1. Implement `IExporter::export_db()` in a new class under [src/lib/libmain/exporters](/src/lib/libmain/exporters). Include `ExportersAliases.h`, keep the class in the `iannotator::exporters` namespace and do not mark it with `IADE_API` - the exporters stay private to the shared object and are reached through `IExporter` only. Should the format need cropped images, take them from `ExportContext::cropper` and fail the export when it is a `nullptr`.
1. Add a `case` for the new enumerator to `iade0impl::LibFactory::create_exporter()` in [src/lib/libmain/LibFactory.cpp](/src/lib/libmain/LibFactory.cpp). The `switch` has no `default`, so a missing case shows up as a compiler warning.
1. Register the new `.cpp` file in the `target_sources()` call of [src/lib/libmain/exporters/CMakeLists.txt](/src/lib/libmain/exporters/CMakeLists.txt) and in the `IADE_EXPORTERS_SRC` list of [src/lib/CMakeLists.txt](/src/lib/CMakeLists.txt). The second list is the one the test executables compile from.
1. Add a unit test directory next to the existing ones under [src/lib/libmain/exporters/tests/unit](/src/lib/libmain/exporters/tests/unit) and `add_subdirectory()` it from that directory's `CMakeLists.txt`. Copying one of the three existing test directories is the quickest start - see [Project tests](/doc/sections/en_US/4-project-structure/4-4-project-tests.md).

The library decodes no image format of its own. Anything that needs pixels - the cropping of the PyTorch vision layout - is delegated to the `IImageCropperFacility` implementation supplied by the consumer.
