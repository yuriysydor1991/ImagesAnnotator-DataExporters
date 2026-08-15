## Where the exporters implementation lives

A call chain through the library is short. A consumer fills the `LibraryContext` descendant of the wanted layout, hands it to `ILib::libcall()` and gets the dataset written to `LibraryContext::export_path`:

- [src/lib/facade/LibraryFacade.cpp](/src/lib/facade/LibraryFacade.cpp) - the installed facade. Every static method simply asks `iade0impl::LibFactory` for the requested object, so that no implementation type appears in a public header.
- [src/lib/libmain/LibMain.cpp](/src/lib/libmain/LibMain.cpp) - the only `ILib` implementation. `LibMain::libcall()` creates the exporter of the given context, stores it back into `ctx->exporter`, copies `export_path`, `dbProvider` and `cropper` into a fresh `ExportContext` and calls `IExporter::export_db()` on it.
- [src/lib/libmain/LibFactory.cpp](/src/lib/libmain/LibFactory.cpp) - the factory. `LibFactory::create_exporter()` asks the given context for its exporter and returns a `nullptr` for an empty one.
- [src/lib/facade](/src/lib/facade) - the `LibraryContext` descendants next to the facade: `PlainTxtExportLibraryContext`, `Yolo4ExportLibraryContext` and `PyTorchExportLibraryContext`. Each one is declared in an installed header and implements `create_exporter()` against the exporter it names, which keeps that exporter out of the public interface.
- [src/exporters](/src/exporters) - `PlainTxt2FolderExporter`, `Yolo42FolderExporter` and `PyTorchVisionFolderExporter`, all in the `iannotator::exporters` namespace, plus `Yolov4CfgWriter`, which the darknet export hands its `cfg/yolov4-obj.cfg` stream to and which emits the whole YOLO v4 network into it. `ExportersAliases.h` pulls the installable interface names and the data drivers record names into that namespace, so the moved application code keeps referring to them unqualified.
- [src/helpers](/src/helpers) - shared by all three exporters: `ImageRecordUrlAndPathHelper` decides whether a record points at a local file or at a URL, `ImageLoader` preloads a web hosted record into a local cache directory, and `TypeHelper` shortens the numeric conversions the coordinate maths needs.
- [src/CURL](/src/CURL) - `CURLController`, the libcurl wrapper `ImageLoader` downloads through.

The helper and CURL namespaces are nested under `iannotator::exporters` on purpose. The data drivers library carries top level namespaces of the same names, and both libraries end up in the same process.

### Adding a new export format

1. Implement `IExporter::export_db()` in a new class under [src/exporters](/src/exporters). Include `ExportersAliases.h`, keep the class in the `iannotator::exporters` namespace and do not mark it with `IADE_API` - the exporters stay private to the shared object and are reached through `IExporter` only. Should the format need cropped images, take them from `ExportContext::cropper` and fail the export when it is a `nullptr`.
1. Declare a new `LibraryContext` descendant in [src/lib/facade/public](/src/lib/facade/public), copying one of the three existing headers. Mark it `IADE_API` and override `create_exporter()`. This is the only installed header the change adds.
1. Implement that `create_exporter()` in a matching `.cpp` under [src/lib/facade](/src/lib/facade), returning the new exporter.
1. Register the new exporter `.cpp` in the `target_sources()` call of [src/exporters/CMakeLists.txt](/src/exporters/CMakeLists.txt) and in the `IADE_EXPORTERS_SRC` list of [src/CMakeLists.txt](/src/CMakeLists.txt), and the new context `.cpp` in the `IADE_CONTEXTS_SRC` list of that same file. Both lists are the ones the test executables compile from.
1. Add a unit test directory next to the existing ones under [src/exporters/tests/unit](/src/exporters/tests/unit) and `add_subdirectory()` it from that directory's `CMakeLists.txt`. Copying one of the three existing test directories is the quickest start - see [Project tests](/doc/sections/en_US/4-project-structure/4-4-project-tests.md).

The library decodes no image format of its own. Anything that needs pixels - the cropping of the PyTorch vision layout - is delegated to the `IImageCropperFacility` implementation supplied by the consumer.
