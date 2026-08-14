### Manual tests run

Every test binary is an ordinary Google Test executable, so it may be started
on its own without `ctest`. The tests are separated into different files named
with the `UTEST_*` pattern for the unit tests and with the `CTEST_*` pattern
for the component ones.

You may search for all compiled available tests by a next command:

```
# from the project root or a build directory

find -type f -executable \( -name 'UTEST_*' -o -name 'CTEST_*' \)
```

After that, choose the particular test of interest and execute it manually if
needed. For example, for the `LibraryFacade` class pass its unit test file
system path into the command line and hit enter (GNU/Linux based):

```
# from the project root

./build/src/lib/facade/tests/unit/LibraryFacade/UTEST_LibraryFacade
```

The exporter tests live next to the sources they cover:

```
# from the project root

./build/src/exporters/tests/unit/PlainTxt2FolderExporter/UTEST_PlainTxt2FolderExporter
./build/src/exporters/tests/unit/Yolo42FolderExporter/UTEST_Yolo42FolderExporter
./build/src/exporters/tests/unit/Yolov4CfgWriter/UTEST_Yolov4CfgWriter
./build/src/exporters/tests/unit/PyTorchVisionFolderExporter/UTEST_PyTorchVisionFolderExporter
```

The `CTEST_Exporters` component test is started the same way. Unlike the unit
tests it links the produced shared library and reaches it through the public
headers only, exactly the way a downstream project does, so it exercises the
real exporters end to end:

```
# from the project root

./build/src/exporters/tests/component/Exporters/CTEST_Exporters
```

The usual Google Test command line flags apply, for instance to run a single
case or to list the available ones:

```
./build/src/lib/facade/tests/unit/LibraryFacade/UTEST_LibraryFacade --gtest_list_tests
./build/src/lib/facade/tests/unit/LibraryFacade/UTEST_LibraryFacade --gtest_filter=*library_version*
```
