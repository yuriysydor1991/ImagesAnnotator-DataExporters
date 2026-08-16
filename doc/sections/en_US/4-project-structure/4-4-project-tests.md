## Project tests

The project carries two kinds of tests, each behind its own CMake option.

The unit tests are built with `-DENABLE_UNIT_TESTS=ON`. They compile the components straight from the tree against the gmock stand-ins under [src/tests/mocks](/src/tests/mocks), `src/lib/facade/tests/mocks` and `src/lib/libmain/tests/mocks`, so a component is exercised with its neighbours replaced:

| Binary | Under test | Sources |
|---|---|---|
| `UTEST_LibraryFacade` | the installed facade | [src/lib/facade/LibraryFacade.cpp](/src/lib/facade/LibraryFacade.cpp) |
| `UTEST_LibFactory` | the object factory | [src/lib/libmain/LibFactory.cpp](/src/lib/libmain/LibFactory.cpp) |
| `UTEST_LibMain` | the `ILib` implementation | [src/lib/libmain/LibMain.cpp](/src/lib/libmain/LibMain.cpp) |
| `UTEST_PlainTxt2FolderExporter` | the plain text exporter | [src/exporters/PlainTxt/PlainTxt2FolderExporter.cpp](/src/exporters/PlainTxt/PlainTxt2FolderExporter.cpp) |
| `UTEST_Yolo42FolderExporter` | the darknet YOLOv4 exporter | [src/exporters/Yolo4/Yolo42FolderExporter.cpp](/src/exporters/Yolo4/Yolo42FolderExporter.cpp) |
| `UTEST_Yolov4CfgWriter` | the darknet network descriptor writer | [src/exporters/Yolo4/Yolov4CfgWriter.cpp](/src/exporters/Yolo4/Yolov4CfgWriter.cpp) |
| `UTEST_PyTorchVisionFolderExporter` | the PyTorch vision exporter | [src/exporters/PyTorch/PyTorchVisionFolderExporter.cpp](/src/exporters/PyTorch/PyTorchVisionFolderExporter.cpp) |
| `UTEST_ImageRecordUrlAndPathHelper` | the record path and URL resolution | [src/helpers/ImageRecordUrlAndPathHelper.cpp](/src/helpers/ImageRecordUrlAndPathHelper.cpp) |
| `UTEST_ImageLoader` | the web hosted image preloader | [src/helpers/ImageLoader.cpp](/src/helpers/ImageLoader.cpp) |
| `UTEST_TypeHelper` | the numeric cast helper | [src/helpers/TypeHelper.h](/src/helpers/TypeHelper.h) |
| `UTEST_CURLController` | the libcurl adaptor | [src/CURL/CURLController.cpp](/src/CURL/CURLController.cpp) |

The `UTEST_ImageLoader` cases stay offline on purpose: they drive the local record, the already cached record and the null record paths, none of which reaches `CURLController::download()`.

The component tests are built with `-DENABLE_COMPONENT_TESTS=ON`:

| Binary | Scope |
|---|---|
| `UTEST_OpenCVImageCropper` | the library's own image cropper, configured only in a build that found OpenCV |
| `CTEST_DefaultLogger` | the logger compiled into the library, driven through its own class |
| `CTEST_Exporters` | compiles none of the library: it links the produced shared object and drives it through the installable public headers only, exactly the way a downstream project does |

`CTEST_Exporters` is the test that would notice a broken install interface - a header left out of the install set, or a symbol that never left the shared object because it lost its `IADE_API` mark.

With both options on the suite counts 61 test cases:

```
# from the project root directory

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=<the data drivers install prefix> \
  -DENABLE_UNIT_TESTS=ON -DENABLE_COMPONENT_TESTS=ON
cmake --build build -j$(nproc)
ctest --test-dir build
```

Every test is registered by `gtest_add_tests()` with a `.noArgs` suffix and a 600 second timeout. See [Enabling unit testing](/doc/sections/en_US/5-project-build/testing/5-3-1-enabling-unit-testing.md) for the build options and [Run tests by the ctest](/doc/sections/en_US/6-running-the-tests/6-3-1-run-tests-by-the-ctest.md) for running and filtering them.
