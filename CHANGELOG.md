# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.11.0] - 2026-08-01

The first release of `ImagesAnnotatorDataExporters` as a project of its own. The
repository started life as the `lib` branch of the
[cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template) project
and has now been filled with the dataset exporters extracted from the
[ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) application,
so that the application and any other tool can turn an annotations project into
a training dataset without duplicating the code.

### Added

- **The Pascal VOC dataset layout**, `PascalVocExportLibraryContext`, written by
  the `PascalVoc2FolderExporter` of the new
  [src/exporters/PascalVoc](/src/exporters/PascalVoc) sub-directory. The export
  is the devkit directory shape: the copied images under `JPEGImages/`, one
  `Annotations/<stem>.xml` descriptor per image over them and the
  `ImageSets/Main` lists naming those. These are the very files
  [LabelImg](https://github.com/HumanSignal/labelImg) writes and reads, which
  makes this the one export of the library that leads back into an annotating
  session, and the torchvision `VOCDetection` and the MMDetection `XMLDataset`
  take the same three directories.
- A rectangle reaches the `bndbox` as the two corner points it was drawn
  between, in the image own pixels: `xmin`/`ymin` is its origin and
  `xmax`/`ymax` that origin plus its size, which is the pair LabelImg turns back
  into the very same rectangle. The 1-based coordinates of the original devkit
  are not written; a reader expecting them shifts the box by a single pixel and
  keeps its size, since both corners move together.
- This is also the one layout of the library that numbers nothing. The class
  name is written into `name` as it stands, so the position of a name in the
  sorted set the database reports decides nothing here and a name added to a
  project renumbers no file exported before it. `depth` is `3` for every image,
  since an `ImageRecord` carries no channel count and the format leaves no way
  to write the size without one, while `pose`, `difficult` and `segmented`
  carry the values of a database which holds no viewing angle, no hard-to-see
  mark and no mask.
- The two guards of the Ultralytics exporters apply here too - a rectangle
  reaching over an image edge is cut down to the image and the edges of a
  negative sized one are sorted before it is cut - and what the cut really did
  remove is reported by the `truncated` flag the format keeps for an object
  continuing past the picture. A rectangle left with no area inside the image is
  logged and dropped, while the image and the rest of its rectangles are
  exported as usual. An annotation name and a file name are both user text, so
  both are written out XML escaped, with the control characters XML 1.0 has no
  way to carry at all dropped rather than written into a file no parser would
  accept.
- The copied image is given its `-1`, `-2`, ... suffix for a taken file **stem**
  and not only for a taken file name, because this is the one layout keying the
  descriptor to the stem: `park.jpg` and `park.png` would otherwise both ask for
  `Annotations/park.xml`. The two `ImageSets/Main` lists hold identical content,
  exactly as the darknet layout writes one and the same list into its `train.txt`
  and `val.txt`, but their names are not free - a reader of this format asks for
  a split by name.
- `LibraryFacade::create_pascal_voc_library_context()`, one per layout as every
  other context factory of this library, and the
  `UTEST_PascalVoc2FolderExporter` unit test with the
  `pascal_voc_export_writes_the_corners_of_the_rectangle` case of
  `CTEST_Exporters`, which drives the new context through the installed headers
  the way a downstream project does.
- **The COCO object detection dataset layout**, `CocoExportLibraryContext`,
  written by the `Coco2FolderExporter` of the new
  [src/exporters/Coco](/src/exporters/Coco) sub-directory. Every layout this
  library could write until now fed a YOLO training run or the PyTorch Vision
  `ImageFolder`; the COCO JSON descriptor is read by Detectron2, MMDetection,
  torchvision, the HuggingFace detection transformers, CVAT, FiftyOne, Label
  Studio and Roboflow. The export is the copied images under `images/` and the
  single `annotations/instances_default.json` over them, which is all any of
  those needs: the images directory is handed over as the dataset root and the
  descriptor names its images by their file name alone.
- It is also the one layout which hands a rectangle over untouched. The `bbox`
  is the `[x, y, width, height]` of the top left corner in the image own
  pixels - the very four numbers an `ImageRecordRect` holds - where every YOLO
  layout divides them by the image size first. The `width` and the `height` of
  the image travel along in the `images` array, so the descriptor is the one
  export of this library that can be read back into the drawn rectangles.
- The identifiers of the three arrays are running counters over what the export
  has really written out and start at `1`, since a reader treats the category
  `0` as the background one. `area` is the box area, `iscrowd` is `0` for a
  named rectangle and `segmentation` is left empty, which is what makes the
  export a detection dataset rather than a segmentation one. An annotation name
  and a file name are both user text, so both are written out JSON escaped.
- The two guards of the Ultralytics exporters apply here for a reason of their
  own: a rectangle reaching over an image edge is cut down to the image and the
  edges of a negative sized one are sorted before it is cut, because the `area`
  written next to such a box, and every overlap ever computed against it, would
  otherwise be a lie. A rectangle left with no area inside the image is logged
  and dropped, while the image and the rest of its rectangles are exported as
  usual.
- `LibraryFacade::create_coco_library_context()`, one per layout as every other
  context factory of this library, and the `UTEST_Coco2FolderExporter` unit
  test with the `coco_export_writes_the_pixel_bbox_of_the_rectangle` case of
  `CTEST_Exporters`, which drives the new context through the installed headers
  the way a downstream project does.
- **The three Ultralytics YOLO dataset layouts**, the ones every YOLO release
  since v5 reads: `UltralyticsDetectExportLibraryContext`,
  `UltralyticsObbExportLibraryContext` and
  `UltralyticsSegmentExportLibraryContext`, written by the
  `UltralyticsDetect2FolderExporter`, `UltralyticsObb2FolderExporter` and
  `UltralyticsSegment2FolderExporter` of the new
  [src/exporters/Ultralytics](/src/exporters/Ultralytics) sub-directory. The
  only YOLO this library could write until now was the darknet directory of
  `Yolo4ExportLibraryContext`, which no Ultralytics release reads: it names its
  classes in `data/obj.names`, lists its images in `data/train.txt` and carries
  the whole network in `cfg/yolov4-obj.cfg`, where v5 and everything after it
  - v8, v11 and the ones since - want a single `data.yaml` descriptor over the
  `images/train` and `labels/train` directories.
- The three of them share every byte of that layout and differ in one line
  only, the label file line of a rectangle, which is what the trained task
  changes: `class centre-x centre-y width height` for the detection, the four
  box corners `class x1 y1 x2 y2 x3 y3 x4 y4` for the oriented boxes and the
  polygon `class x1 y1 ... xn yn` for the segmentation. That one line is the
  single abstract method of the shared `Ultralytics2FolderExporter` base, which
  holds the directory, the descriptor, the image copying and the
  normalisation. An annotations database of axis aligned rectangles has nothing
  to tell the oriented box file and the polygon file apart, so those two
  coincide today - what differs is the training task which reads them.
- Two guards the darknet exporter does not have, because an Ultralytics release
  refuses a whole image over a single label coordinate outside of the `0..1`
  range: a rectangle reaching over an image edge is cut down to the image, and
  the edges of a rectangle drawn from the right or from the bottom - one
  carrying a negative width or height - are sorted before it is cut. A
  rectangle left with no area inside the image at all is logged and dropped,
  while the image and the rest of its rectangles are exported as usual.
- The `data.yaml` descriptor carries the annotation names in the YAML single
  quoted style, so a colon, a hash or a quote inside an annotation name stays a
  part of the name instead of turning into syntax. Its `path` is the export
  directory written out absolute, so that the descriptor resolves whatever the
  working directory of the training run is; dropping that one line makes the
  directory relocatable, since an Ultralytics release then falls back to the
  directory holding the descriptor. `train` and `val` both name `images/train`,
  so the whole set is offered for the validation as well, exactly as the
  darknet layout writes one and the same list into `train.txt` and `val.txt`.
- `LibraryFacade::create_ultralytics_detect_library_context()`,
  `create_ultralytics_obb_library_context()` and
  `create_ultralytics_segment_library_context()`, one per layout as every other
  context factory of this library, each handing out the pointer type of its own
  layout.
- `UTEST_Ultralytics2FolderExporter` over the three of them, and the
  `ultralytics_detect_export_writes_the_data_yaml_layout` and
  `ultralytics_obb_export_writes_the_four_box_corners` and
  `ultralytics_segment_export_writes_the_box_polygon` cases of
  `CTEST_Exporters`, which drive the new contexts through the installed headers
  the way a downstream project does. The suite counts 122 cases now.
- The pose and the classification layouts of the YOLO family stay unwritten on
  purpose, and the produced dataset layouts documentation section says so: a
  pose label line carries the keypoints of the object, which the annotations
  database does not hold at all, and the classification layout is the PyTorch
  Vision `ImageFolder` one this library already writes with a train and
  validation split directory added on top of it - and that split is the very
  thing this library leaves to its consumer.
- **An image cropper of the library's own, built on OpenCV.** The library
  decodes no image format of its own, which is why the PyTorch Vision export
  asks its consumer for an `IImageCropperFacility`. A consumer with no imaging
  stack to lend was stuck; now `iannotator::exporters::croppers::OpenCVImageCropper`
  answers that need, reached through `LibraryFacade::create_image_cropper()`.
  It clamps a rectangle reaching over an edge, writes PNG, rewrites the
  destination extension and never overwrites an existing crop - the semantics
  of the cutters the ImagesAnnotator application supplies.
- The dependency is optional in both directions, through the new
  `template-project-OpenCV-enabler` and its companion linker module, taken from
  the appOpenCV branch of the project template and turned from a
  `find_package(REQUIRED)` into a probe. A system without OpenCV, or a
  `-DENABLE_OPENCV=OFF`, only means the library is built without that cropper:
  the configure succeeds and says so, and the export keeps asking its consumer
  for a cropper as it always did. The always compiled
  `croppers::create_builtin_cropper()` is what turns that build time condition
  into a run time `nullptr`, so no caller repeats the `#ifdef`.
- The resolution order of the PyTorch Vision export, in one line: a cropper the
  consumer supplied always wins. The library's own only fills an empty
  cropper slot, so an existing consumer - the GUI cutters of
  the annotator application among them - sees no change whatsoever, and a new
  one may simply leave the field alone. Only when both are absent does the
  export fail, exactly as before.
- No OpenCV type reaches an installed header: the cropper is handed out as an
  `IImageCropperFacility` like any consumer supplied one, so a project
  consuming this library needs no OpenCV of its own even when the library was
  built with it.
- `UTEST_OpenCVImageCropper`, ten cases over the cropper itself, configured
  only in a build that found OpenCV - every case drives OpenCV, and
  `gtest_add_tests()` registers cases by scanning the source text rather than
  the binary, so an `#ifdef` inside the file would have registered cases the
  binary does not hold and ctest reports a filter matching nothing as a pass.
  The build without OpenCV is covered instead by
  `UTEST_LibFactory.create_image_cropper_matches_what_the_build_found` and by
  `UTEST_PyTorchVisionFolderExporter.handles_a_missing_cropper_by_the_build`,
  both of which are compiled in every configuration.
- The three dataset exporters, moved out of the ImagesAnnotator
  `src/annotator-business/exporters` component: `PlainTxt2FolderExporter`,
  `Yolo42FolderExporter` and `PyTorchVisionFolderExporter`, together with the
  dependency closure they need - `ImageLoader`, `ImageRecordUrlAndPathHelper`,
  `TypeHelper` and `CURLController`.
- The installable public interface under
  [src/lib/facade/public](/src/lib/facade/public), in the version stamped
  `ImagesAnnotatorDataExporters011` namespace: `IExporter`,
  `IImageCropperFacility` and the extended `LibraryFacade`, `LibraryContext`
  and `ILib`.
- `LibraryContext` is the single data class of that interface. The very same
  instance names the layout, carries the export in-data and receives the
  exporter of the run, whether it drives the one shot `ILib::perform_export()`
  or the `IExporter::export_db()` of an exporter built by hand, so nothing has
  to be copied from one context class into another. Its data members are
  private, reached through the `get_export_path()` / `set_export_path()` styled
  accessor pairs, so the class may grow a check or a default without touching a
  single call site. It holds only what every layout needs: the image cropper
  sits on `PyTorchExportLibraryContext`, the one layout which cuts pixels out,
  so no other export carries a slot it never reads.
- The wanted dataset layout is named by the `LibraryContext` descendant the
  consumer instantiates - `PlainTxtExportLibraryContext`,
  `Yolo4ExportLibraryContext` or `PyTorchExportLibraryContext`. The type itself
  is the layout, and `LibFactory::create_exporter()` maps it onto the concrete
  exporter; a descendant carries data only when its layout asks for data no
  other one needs. That replaces the `ExportFormat`
  enumeration the context used to carry, so no unknown format value can be
  constructed at all.
- `LibraryFacade::create_exporter()` and `library_version()` as the entry
  point of the library, plus the one shot `ILib::perform_export()`, both driven
  by a `LibraryContext`.
- One `LibraryFacade` context factory per dataset layout -
  `create_plain_txt_library_context()`, `create_yolo4_library_context()` and
  `create_pytorch_library_context()` - each handing out the pointer type of its
  own layout, so the cropper setter of the PyTorch Vision one is reachable
  without a cast. They replace the single `create_library_context()`, which
  handed out the base pointer of a default layout.
- `IImageCropperFacility`, implemented by the consuming project: the library
  decodes no image format of its own, so the PyTorch Vision export asks its
  consumer to cut the rectangles out.
- Unit tests for every moved exporter and a `CTEST_Exporters` component test
  that links the real shared library and drives it through the public headers
  only, over a database built with the data drivers library.
- Unit tests for the rest of the moved code, which came over without any:
  `UTEST_ImageRecordUrlAndPathHelper`, `UTEST_ImageLoader`, `UTEST_TypeHelper`
  and `UTEST_CURLController`. The suite counts 61 cases now. The image loader
  cases stay offline: they drive the local, the already cached and the null
  record paths, none of which reaches a download.
- A `Dependencies` stage in the Jenkins pipeline that builds and installs
  ImagesAnnotatorDataDrivers, with the `DATA_DRIVERS_PREFIX` and
  `DATA_DRIVERS_REF` parameters.
- The quick build scripts of the
  [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template) `lib`
  branch in the new `scripts` directory, so a whole `Release` or `Debug` cycle
  is a single command instead of the separate CMake calls. `scripts/build`
  holds the three stage scripts of each build type (`*-configure.sh` /
  `*-build.sh` / `*-install.sh`, keeping their trees at `build/release` and
  `build/debug`), the two entry points chaining them (`release.sh`, `debug.sh` -
  the latter configuring with the tests, the documentation, `MAX_LOG_LEVEL=5`
  and `cppcheck` on) and the five analysis entry points (`debug-cppcheck.sh`,
  `debug-clang-tidy.sh`, `debug-sanitizers.sh`,
  `debug-sanitizers-threads.sh`, `debug-compiler-analyzer.sh`);
  `scripts/docker` holds the two starter scripts of the
  `jenkins-pipeline-docker-{build,run}` targets, which are the only container
  targets this project kept. They consume two parameters of their own -
  `--no-reconfigure`, which keeps the existing build directory instead of
  erasing it, and `--install`, without which the `sudo cmake --install` step
  never runs and no password is ever asked - and forward every
  `-D<variable>=<value>` to the configure step, which is also how a data
  drivers installation outside the default CMake prefixes is named
  (`-DCMAKE_PREFIX_PATH=<prefix>`). Documented in the new `5-38` sections in
  both languages, which name the data drivers library the configure step
  requires and cannot fetch, with the starter scripts in `5-17` and the
  `--install` shortcut in `7-installing`.

### Changed

- CMake project renamed from `CppAppTemplate` to `ImagesAnnotatorDataExporters`;
  the produced library is `libImagesAnnotatorDataExporters-0.11.so` and its
  headers install under `include/ImagesAnnotatorDataExporters-0.11/`.
- `LIB_INCLUDE_MINOR_IN_NAME` now defaults to `ON`, so the installable names
  carry the same major and minor pair the `ImagesAnnotatorDataExporters011`
  public namespace does, exactly as the data drivers library names itself.
- The records the exporters read are no longer defined here: the library depends
  on [ImagesAnnotatorDataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers)
  and takes `IImagesPathsDBProvider`, `ImageRecord` and `ImageRecordRect` from
  it. It is resolved with `find_package` by the new
  `template-project-data-drivers-enabler` module and linked **PUBLIC** by its
  `-linker` companion, because the installable headers name those types.
- `ENABLE_LIBCURL` now defaults to `ON` and is mandatory, and its enabler was
  split into an `-enabler` module, which makes libcurl available before `src/`
  is added so the tests may link it, and a `-linker` module, which links it
  `PRIVATE` against the library target afterwards.
- The library is built with `CXX_VISIBILITY_PRESET hidden` and only the new
  `IADE_API` marked interface of `ExportersAPI.h` is exported. This is a
  correctness fix, not an optimisation - see below.
- The moved `helpers` and `curli` implementation namespaces are nested under
  `iannotator::exporters` instead of staying at the top level, for the same
  reason.
- `LibMain::perform_export()` is a real implementation now: it builds the
  exporter of the context layout, runs it and publishes it through
  `LibraryContext::set_exporter()`.
- All the documentation, both `en_US` and `uk_UA`, was rewritten to describe
  this library, with new sections on the exporters API, the produced dataset
  layouts, the data drivers dependency and the downstream integration, a redrawn
  class diagram and the matching Simplified BSD LICENSE.
- Both `README.md` and its `doc/README.uk_UA.md` translation were rewritten from
  the template text: what the library is, its features, a compiled and run usage
  example, the dependencies, the build and test commands, where the code came
  from, and a documentation contents list that reaches the new sections.
- The logging component is the `default-logger` one of the cpp-app-template
  `lib` branch, and the `simple-logger` implementation is gone. `src/log/log.h`
  resolves to `default_logger::DefaultLogger` and `CTEST_SimpleLogger` gives way
  to the `CTEST_DefaultLogger` of the same eleven cases. The object library
  keeps the `POSITION_INDEPENDENT_CODE` and the `CXX_VISIBILITY_PRESET hidden`
  the logger needs inside this shared object, so its symbols stay private here
  exactly as before, and it keeps being linked `PRIVATE` into the library.
- The implementation components moved out of `libmain` and up to the `src`
  level, so that `libmain` carries only the `LibMain` / `LibFactory` entry
  pair: `src/exporters`, `src/croppers`, `src/helpers` and `src/CURL`. The
  exporters component is split one sub-directory per dataset layout -
  `PlainTxt`, `Yolo4` (the exporter and its `Yolov4CfgWriter`) and `PyTorch` -
  each a sub-component with its own `CMakeLists.txt`, its own source list
  (`IADE_PLAIN_TXT_SRC`, `IADE_YOLO4_SRC`, `IADE_PYTORCH_SRC`, united into
  `IADE_EXPORTERS_SRC`) and its own `tests/unit` tree, while the shared
  `ExportersAliases.h` and the `CTEST_Exporters` component test driving all
  three layouts stay at the component root. The
  per-component source lists the test executables compile from moved along
  with them, from `src/lib/CMakeLists.txt` up to `src/CMakeLists.txt`, the
  directory every component is now a child of. The data drivers library is
  laid out the same way.

### Fixed

- **The darknet export wrote no network to train.** `cfg/yolov4-obj.cfg`, the
  descriptor `obj.data` sends darknet to, held two lines - `classes` and
  `filters` - and not a single section, so darknet had neither the network
  geometry nor one layer to build: `darknet detector train` died on it with a
  segmentation fault, the parser reaching for a first section that was not
  there. The export now writes the whole YOLO v4 network of the darknet
  `cfg/yolov4-custom.cfg` - the CSPDarknet53 backbone, the SPP and PANet neck
  and the three detection heads, 162 layers - and darknet trains out of an
  exported directory as it stands, given the `yolov4.conv.137` backbone
  weights. Everything the project decides is filled in: the class count into
  the three `[yolo]` layers and the `(classes + 5) * 3` filters of the
  convolution in front of each of them, the iterations count as the 2000 per
  class of the darknet manual, never fewer than 6000 and never fewer than the
  images the export has copied, with `steps` at its 80% and 90%. The input is
  832 by 832, so that the small objects of a big photo still reach the
  detectors, and `subdivisions=32` keeps that input within a common video
  memory. The network is emitted by the new `Yolov4CfgWriter` out of the blocks
  it repeats - one residual block 23 times, one five convolutions block four
  times - rather than kept as a thousand line copy of the darknet file, and the
  two layer indices the neck routes back into are counted while writing instead
  of being written down. The descriptor is now written last of the export, since
  the images it counts are copied by then. `UTEST_Yolov4CfgWriter` covers the
  writer with four cases, and `UTEST_Yolo42FolderExporter` now asserts the
  written descriptor really holds the three detectors of the project class
  count.
- **The project did not configure against a stock data drivers install.** The
  data drivers library ships with `LIB_INCLUDE_MINOR_IN_NAME=ON`, so it installs
  as `ImagesAnnotatorDataDrivers-0.11`, while the enabler here asked for
  `ImagesAnnotatorDataDrivers-0` and the sources included
  `<ImagesAnnotatorDataDrivers-0/...>`. Every build against a default dependency
  install stopped at `find_package`, the Jenkins `Dependencies` stage included,
  since that stage builds the dependency with its defaults.
- The exporter unit tests were declared with no `ENABLE_UNIT_TESTS` guard, so a
  plain configure without the testing options reached
  `target_link_libraries(... GTest::gtest_main)` for a target the GTest enabler
  had never defined and failed to generate.
- **Symbol clash with the data drivers library.** Both libraries are built from
  the same project template and each defined `lib0impl::LibFactory`,
  `lib0impl::LibMain`, `simple_logger::SimpleLogger` and the `project_decls`
  constants - 75 exported symbols in common. A program linking both had one
  library's calls bound to the other library's definitions, and since the two
  `lib0impl::LibFactory` classes do not share a vtable layout, the data drivers
  library crashed on a null slot. Hidden visibility plus the `IADE_API` marked
  public interface removes every non template collision. It does not reach the
  factory: a `std::make_shared` instantiation names its class in its own
  mangled name and stays weak and exported whatever the visibility is, so the
  implementation namespace of this library is `iade0impl` instead of the
  template's `lib0impl`.
- The simple logger object library was never linked into the shared library,
  which shipped unresolved `simple_logger::SimpleLogger` symbols.
- The installable CMake package was broken: the generator overwrote the
  configured `Config.cmake` with the raw exported targets file and wrote the
  version file under a name `find_package()` never looks for, so no versioned
  `find_package()` could ever succeed and no public dependency was pulled in.
  It now installs a real `Config.cmake` that `find_dependency()`s the data
  drivers, plus a correctly named `ConfigVersion.cmake`. The template was
  renamed `TemplateLibraryConfig.cmake.in` -> `ExportersLibraryConfig.cmake.in`.
- The FreeBSD packager built its `pkg` origin from the undefined
  `PROJECT_BINARY_NAME_lower` variable, leaving the origin without its name
  part.
- The Doxyfile `INPUT` no longer points at a hardcoded `build/` path.
- `CPACK_DEBIAN_PACKAGE_SHLIBDEPS` is now `ON`, so the libcurl and data drivers
  dependencies are read out of the produced binary instead of being left empty.

### Removed

- **The build system and documentation this library does not use.** The tree
  came from the project template with the whole catalogue of optional third
  party enablers, and the exporters include none of them: the only external
  headers under `src/` are `curl/curl.h`, the
  `<ImagesAnnotatorDataDrivers-0.11/...>` ones and GoogleTest. Gone are the
  sixteen enabler modules for the ten image libraries, zlib, liblzma, nlohmann
  json, libxml2, OpenCV and OpenSSL, their `include()` lines, their two
  `CMAKE_MODULE_PATH` entries and their twenty eight documentation sections.
- The documentation of an application this project does not build: it produces
  a library and its test executables, and defines no `PROJECT_BINARY_NAME` at
  all. The "searching for" and "starting the generated executable" sections and
  the "changing the project and the executable name" one are gone, and
  `6-run-the-executable` is now `6-running-the-tests`, holding the two test
  running sections that were always its only applicable content.
- The template workflow documentation - cloning the template, forking and
  replacing the origin, the extensions catalogue and the minimal possible
  versions - which describes the template rather than this library.
- The installed binary, its public headers and its CMake package are unchanged
  by all of the above: the exported symbol lists compare equal.
- The template's placeholder `LibMain` demonstration body and the "template
  project flavors" branch catalogue in the READMEs.
- `misc/packagers/flatpak.conf.json.in`, an orphan left by the template: no
  CMake module in this branch ever referenced it.
- The Jenkins pipeline snap packager stage and its `RUN_SNAP_PACKAGER`
  parameter: this branch ships no snap enabler for them to drive.
- The `src/tests/mocks/LibraryContext` gmock stand-in, no longer needed now that
  `LibraryContext` is header only.

# Inherited cpp-app-template history

Everything below happened in the
[cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template) `lib`
branch, from which this repository was forked. It is kept for the record
because the git history of this repository still contains those commits.

## [lib0.10.0] - 2025-09-08

- ea177d5 Including the main Config.cmake file into installation candidates
- 2c3ba48 Library infrastructure reorg. Introducing basic installable CMake module.
- 3d8dc47 Adding money emoji to donation notice in the READMEs
- d4cff25 Introducing the appCURL branch info into the READMEs
- 3b7532b Introducing the enabling libcurl section into the READMEs
- ff3563b Introducing the libcurl cmake enabler script
- 28c0524 Moving the DEFAULT_LOG_FILE_PATH macro declaration into severity-macro-consts.h
- f71398f Applied clang-format
- 1e8e770 Introducing the DEFAULT_LOG_FILE_PATH cmake var and cpp macro to specify default dst log file location
- 49ddb64 Introducing some marketing sentences in the README's goal section
- bf79464 Introducing the cmake/enablers subdirectory to locate the 3rd-party dependencies
- 3190db6 Renaming emplate-project-make-GTest-available - template-project-GTest-enabler
- c18581a Introducing the NLohmann JSON library enabler cmake module
- 29f0505 Avoiding unnecessary badly portable localtime_r usage
- 6ec8b3c Introducing the thread id into simple logger
- 4043abe Avoiding backward time in the simple logs
- b72fc91 Avoiding final log multiple str fetches
- ba132bb Refining the log current timestamp method, increasing thread safety
- 171ea6d Introducing the CMake configurable log severity messages compilation switch
- 4d3757f Hiding the compile options cmake files under separate directory
- ada1be2 Incrementing version to 0.10.0

## [lib0.9.0] - 2025-06-06

- 6e185e4 Incrementing version to 0.9.0
- 45f0e36 Release lib0.9.0 2025-06-06
- cf3b980 Adding project neatness to the READMEs
- f1ed4bb Introducing the appLog4Cpp5 branch info into the READMEs
- 16de7ad Implementing the logging macros and a simple logger to perform log messaging
- 141ad3e Introducing the appMongoDBCpp4 branch info into the READMEs
- 29bbeb5 Refining the deb cmake info for cpack
- d5cf23c Introducing the appSQLiteCpp3 branch info into the READMEs
- 3396ed4 Introducing the gitlab mirror into the READMEs
- 3da8265 Introducing the appMySQLCppConn branch info into the READMEs
- 968858a Introducing the appBoostBeast branch info into the READMEs
- 5fa9629 Explicitly specifying the combining of the template branches
- 46c5190 introducing the appPgSQLxx branch into the READMEs
- 57311dc Introducing a new appWt4 branch info into the READMEs
- dd85971 Introducing the error exit code for a cppcheck target
- 0b7856e Refining the cppcheck target
- a564f86 Introducing the donation notice into the READMEs
- 90b206a Introducing the appMeson branch info into the READMEs
- 9af4522 Refining branches info
- c75b2a2 Introducing the app branch and refine the master branch
- 7ce5298 Revert "Introducing the flatpak package creation from the template source."
- d029b40 Revert "Introducing the flatpak sections into the READMEs"
- 6b76d82 Introducing the flatpak sections into the READMEs
- 763969f Introducing the flatpak package creation from the template source.
- 847fe28 Erasing the /analyze from the default MSVC compile options
- 875bc01 Introducing MSVC extra compile options for warnings
- 689fcd7 Introducing the DEB package gen info in the READMEs
- 21a6522 Introducing the cpack configuration for the deb package generation
- 14dbc41 Introducing the LICENSE file with free of use license and notice
- b0de6da Introducing the appSDL2 branch into the READMEs
- 233c0b2 Introducing clang-tidy
- 52f1336 Erasing redundant libraries installation info
- 70f8f1e Introducing the FreeGlut/OpenGL appFreeGlut branch info into READMEs
- 5d3aae8 Introducing MS Windows tools install description into READMEs
- 69e59f7 Adding the out directory for MSVC into .gitignore
- e069e8e Specifying the appGtkmm3Glade branch in the READMEs
- 038f0e5 Introducing the appGtkmm3 branch info into the READMEs

## [lib0.8.0] - 2025-03-16

- b6dd57f Release lib0.8.0 2025-03-16
- 7081e52 Erasing merging symbols.
- db6d05c Erasing app infrastructure classes mention in the LibraryContext
- 98d8785 Introducing the lib branch info into the READMEs
- 6207e8f Refining the READMEs to represent library only template project
- dad1ed9 Erasing all the application binary infrastructure.
- 7e8d539 Introducing the appQt6 branch in READMEs
- a6d72dc Introducing Doxygen doc with translated README as main page facility
- 73e5d0d Introducing COMPILE_WARNINGS_AS_ERRORS and ENABLE_COMPILER_CODE_ANALYZER CMake vars.
- d4c6dc1 Incrementing version to 0.8.0
- ead330e Inserting forgotten release link in the CHANGELOG

## [applib0.7.0] - 2025-03-14

- f5b1d13 Release applib0.7.0 2025-03-17
- 473bb31 Refining typos in uk_UA README
- ab45738 Refining all the READMEs
- cfa03d8 Introducing the sample LibMain UTs
- c019bd3 Refining lib UTs
- 2230643 refining LibraryFacade tests
- 5b23447 Introducing LibFactory UTs
- f67a3ca Specifying the library targets and formatting the code.
- 11f5b6d Introducing LibraryFacade UTs.
- 95092ff Moving ILib to the facade folder.
- d02674e Incrementing version to 0.7.0 and changing description

## [applib0.6.0] - 2025-03-13

- 3b43b68 Release applib0.6.0 2025-03-13 21:54
- 8635ccb Release applib0.6.0 2025-03-13
- d0a0eed Introducing explanatory comments for new LibraryContext class fields.
- 34988bb Introducing ApplicationContext2LibraryContext UTs with moved general mocks
- f83084b Removing forgotten debug #error
- d978ec1 Specifying translation documents for the root README under the doc
- c41e944 Specifying the source file for the PROJECT_BINARY_NAME CMake variable
- 78e90c7 Some README.md refinements
- 1f46f37 Refining SOLID comment for Implement code straight away
- 89de1b3 Introducing the ApplicationContext2LibraryContext class with mocks and refined Application UTs
- 492817a Introducing additional library related info into README.md
- 6f48e44 Marking applib branch as current in the README.md
- 522ccfc Introducing multibranch info into the root README.md
- 3f11e47 Marking LibraryContext destructure as virtual to allow subclassing
- 5a8b53b Renaming library facade namespace to templatelib0
- 27bc62a Fixing wrong article.
- 96e24aa Applying clang-format
- 71c3c6d Refining UTEST_Application unit test to pass.
- c223375 Introducing library mocks for the app component
- 45cb5fb Refining library classes explanatory comments.
- 2d2c974 Marking library facade cpp file as private.
- 8cac51b Introducing library component facade and default implementation.
- e19aef1 README.uk_UA.md links refinement
- ec31437 The root README.md file translation to uk_UA workflow final
- be2dd4d The root README.md file translation to uk_UA workflow (pre Installing executable)
- 421f046 The root README.md file translation to uk_UA workflow (pre Run section)
- 86ac202 The root README.md file translation to uk_UA workflow (pre clang-format)
- a443a83 The root README.md file translation to uk_UA workflow (pre enabling testing)
- 8da8536 The root README.md file translation to uk_UA workflow.
- cadecf3 Introducing translated root README in uk_UA.
- a5f282c Incrementing version to 0.6.0

## [app0.5.0] - 2025-03-12

- c60779c Release app0.5.0 2025-03-12
- edad5ce Specifying the cmake modules for the valgrind and cppcheck targets
- cc7c0e7 Specifying particular test run with ctest
- 2fe49df Introducing the Valgrind target doc for root README.md
- ee75ed3 Introducing the cppcheck doc into root README.md
- 2edc812 Fixing cppcheck target exec doc in the README.md
- 2058fe2 Introducing the Valgrind memory check target
- 37b2e48 Decomposing feature enabling if statements into separate file
- 5f96b82 Integrating the cppcheck code checking tool.
- c6aca03 Incrementing version to 0.5.0

## [app0.4.0] - 2025-03-10

- b15973a Release app0.4.0 2025-03-10
- 6329ab2 Sections reorder
- 40b7fdc Introducing clang-format doc into the root README.md
- 6dc997d Setting default OFF value for the clang-format target
- 4d9d227 Fixing absent cmake-modules search path from root
- 38f0c43 Decomposing root CMakeLists.txt var declares into separate module.
- ce9cd15 Adding the clang-format target
- c3aea63 Applied clang-format code formatter
- 6ec1125 Incrementing version to 0.4.0
- c3511ae Introducing Google-based .clang-format file.

## [app0.3.0] - 2025-03-08

- 7dacafc Release app0.3.0 2025-03-08
- e673166 Removing Doxygen mention from the extensions and it's refine.
- 21599fa Specifying optional packaged in the README.md file
- d301440 Introducing the Doxygen doc install feature with custom html name.
- a46f413 Adding README.md to the Doxygen documentation.
- 5de5b7a Adding description for the app namespace
- 1d60e08 Erasing %Z term for the PROJECT_CONFIGURE_DATE variable value format.
- b9b05ba CMake batch replace to a minimum version downgrade.
- 77d99d5 Making Doxygen output directory customizable
- ff46570 Refining README.md
- b5777ed Introducing the Doxygen subsection in the README.md
- 8665554 Refining some classes comments.
- f53a237 Refining Doxygen target
- 500ccf0 Introducing the Doxygen documentation creation target.
- 37ed418 Refining tests run section in the README.md
- bfd7e38 Fixing README.md type
- e0e7225 Fixing type in the CHANGELOG.md
- dfbf4ac Moving ENABLE_UNIT_TEST check to toplevel app UTs
- 21335bf Introducing a new PROJECT_BINARY_NAME CMake-var and CMake files little reorg
- d1f5ead Incrementing version to a 0.3.0

## [app0.2.0] - 2025-03-07

- 74a55dc Release app0.2.0
- 150537c Fixing annoying -fanalyzer caused prolonged build
- 84d7c95 Fixing flags skipping in the CommandLineParser
- 57f0cef Causing to print help if missing command line parameter data.
- c7714dc A bit of a code formatting
- 72fed6d Newline in the project-global-decls.h.in
- dd6dccc Erasing the INVALID static field from App Factory and reusing one from the IApplication
- f2bd6f6 Introducing correct brief comment for the CommandLineParser class
- 905d423 Introducing return value explanation for an abstract run method
- 01e3f80 Fixing main.cpp comment mistake.
- 8f8bb4c Introducing comment to a IApplication INVALID static field
- 30aead4 Introducing comprehensive help message in the ApplicationHelpPrinter
- 19891ac Introducing sample ApplicationFactory UTs.
- 5401442 Introducing a sample CommandLineParser UTs.
- 5c77ee5 Introducing ApplicationVersionPrinter UTs.
- 848bea1 Adding ApplicationHelpPrinter sample UTs.
- eef8dd4 Adding ApplicationContext UTs.
- ff2adf4 Fixing absent includes for ApplicationContext mock
- 8096646 Fixing double Cpp in the binary name for README.md
- 7de3038 Introducing test run commands to the README.md
- e6a30c6 Enabling system GTest probe by the default
- 5967b0b Mentioning GTEST_TRY_SYSTEM_PROBE in the README.md
- 285b387 Adding GTest system probe code to CMakeLists.txt files.
- eebc595 Disabling the UTs by default.
- 458c11f Specifying command execution dir in README.md
- b749982 Refining README.md
- 487748a Integrating GTest and implementing a simple Application test.
- 1515ece Fixing gtest package requirement
- a46c1ba Fixed ApplicationFactory indentation issue.
- 833bc70 Specifying install commands directory in README.md
- e9b4d47 Adding installation section in the README.md
- a216d70 Adjusting execute_process command indentation.
- f4eb95e Adding binary install command.
- 6ead0f8 Refining set origin section in README.md
- 1cc9166 Introducing new info into the README.md
- a3ca641 Incrementing version to 0.2.0

## [v0.1.0] - 2025-03-04

- feded33 Refining CHANGELOG.md
- eaaee95 0.1.0 release
- 522b6b3 Adding changelog
- 5c75937 Refining the README.md file
- f8df49b Introducing additional info into the root README.md file
- a76890a Decomposing the compile options
- d5da609 Adding some comments.
- 28772c6 Introducing h-configure file for version printer
- b5fceb1 Introducing version printing application ApplicationVersionPrinter
- 6d4b9a0 Adding some comments to parser and applications
- ba2bf07 Introducing and integrating a CommandLineParser.
- 720500f Adding a basic help printer application class.
- 7429e21 Adding if-guards for created app instances.
- e2be348 Adding main application function explanatory comment.
- 8b6f9ef Adding ApplicationFactory class and integrating into main.cpp
- 6bbff32 Adding the default application's class Application
- c0debf2 Adding ApplicationContext and IApplication.
- 5a2fb71 Adding basic CMakeLists.txt files structure.
- f770df2 Adding a basic main.cpp
- 5876319 Adding the initial README.md
- 3f53c17 Adding .gitignore

# Releases

[unreleased](https://github.com/yuriysydor1991/cpp-app-template)

[lib0.10.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/lib0.10.0)
[lib0.9.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/lib0.9.0)
[lib0.8.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/lib0.8.0)
[applib0.7.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/applib0.7.0)
[applib0.6.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/applib0.6.0)
[app0.5.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/app0.5.0)
[app0.4.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/app0.4.0)
[app0.3.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/app0.3.0)
[app0.2.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/app0.2.0)
[v0.1.0](https://github.com/yuriysydor1991/cpp-app-template/releases/tag/v0.1.0)
