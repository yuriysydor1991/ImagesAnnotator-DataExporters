### Searching for the generated binaries

The build tree mirrors the source tree, so the shared library is left in the
`src/lib/` sub-directory of the build directory. Locate it with the `find`
command (GNU/Linux based):

```
# from the project root, for a build directory named build

find build -name 'libImagesAnnotatorDataExporters-0.11.so*'
```

which reports the library and its two version links:

```
build/src/lib/libImagesAnnotatorDataExporters-0.11.so
build/src/lib/libImagesAnnotatorDataExporters-0.11.so.0
build/src/lib/libImagesAnnotatorDataExporters-0.11.so.0.11.0
```

The `ImagesAnnotatorDataExporters-0.11` name is composed from the CMake project
name and its major and minor version. It changes if the name segments are
customised, see
[Customizing library name segments](/doc/sections/en_US/5-project-build/compression/5-23-customizing-library-name-segments.md).

The test binaries are built only with `ENABLE_UNIT_TESTS` and/or
`ENABLE_COMPONENT_TESTS` switched on. Search for them by their `UTEST_` and
`CTEST_` prefixes:

```
# from the project root

find build -type f -executable \( -name 'UTEST_*' -o -name 'CTEST_*' \)
```

With both options on the command lists all eight of them:

```
build/src/lib/facade/tests/unit/LibraryFacade/UTEST_LibraryFacade
build/src/lib/libmain/tests/unit/LibFactory/UTEST_LibFactory
build/src/lib/libmain/tests/unit/LibMain/UTEST_LibMain
build/src/lib/libmain/exporters/tests/unit/PlainTxt2FolderExporter/UTEST_PlainTxt2FolderExporter
build/src/lib/libmain/exporters/tests/unit/Yolo42FolderExporter/UTEST_Yolo42FolderExporter
build/src/lib/libmain/exporters/tests/unit/PyTorchVisionFolderExporter/UTEST_PyTorchVisionFolderExporter
build/src/lib/libmain/exporters/tests/component/Exporters/CTEST_Exporters
build/src/log/simple-logger/tests/component/SimpleLogger/CTEST_SimpleLogger
```

None of these files is an application: the `UTEST_`/`CTEST_` binaries are test
runners, and the library itself is loaded by the program that links it.
