### Run tests by the ctest

The tests are built only when the project was configured with the
`ENABLE_UNIT_TESTS` and/or the `ENABLE_COMPONENT_TESTS` variable switched on:

```
# from the project root

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers install prefix> \
      -DENABLE_UNIT_TESTS=ON -DENABLE_COMPONENT_TESTS=ON
cmake --build build -j$(nproc)
```

With both options on the suite holds 246 test cases and all 246 pass. Run them
with the `ctest` command from the project build directory (GNU/Linux based):

```
# run from the project build directory

ctest
```

Alternatively, run the `ctest` command from any location by specifying the test
directory (GNU/Linux based):

```
ctest --tests-dir /path/to/the/project/build/directory
```

The cases are distributed over thirty one binaries: twenty seven unit test ones
(`UTEST_IADataExportersFacade`, `UTEST_LibFactory`, `UTEST_LibMain`,
`UTEST_PlainTxt2FolderExporter`, `UTEST_Yolo42FolderExporter`,
`UTEST_Yolov4CfgWriter`, `UTEST_Ultralytics2FolderExporter`,
`UTEST_Coco2FolderExporter`, `UTEST_PascalVoc2FolderExporter`,
`UTEST_CreateML2FolderExporter`, `UTEST_Dxxwjz2FolderExporter`,
`UTEST_Dxxwjz2NetworkWriter`, `UTEST_Dxxwjz3FolderExporter`,
`UTEST_Dxxwjz3NetworkWriter`, `UTEST_Dxxwjz3CfgJson`,
`UTEST_Dxxwjz2pProjectExporter`, `UTEST_Dxxwjz3pProjectExporter`,
`UTEST_Dxxwjz2pProjectWriter`, `UTEST_Dxxwjz3pProjectWriter`,
`UTEST_Dxxwjz2pAnnotationsWriter`,
`UTEST_PyTorchVisionFolderExporter`, `UTEST_OpenCVImageCropper`,
`UTEST_ImageLoader`, `UTEST_ImageRecordUrlAndPathHelper`, `UTEST_TypeHelper`,
`UTEST_ZlibController`, `UTEST_CURLController`) and four component test ones (`CTEST_Exporters`,
`CTEST_LibraryRealLogger`, `CTEST_DefaultLogger`, `CTEST_ZlibController`). List them without running
anything:

```
# from the project build directory

ctest -N
```

In order to run the cases of one particular binary, pass its name after the
`-R` command line flag. For example, for the `UTEST_IADataExportersFacade` test it'll
look something like this:

```
# from the project build directory

ctest -R UTEST_IADataExportersFacade
```

The same flag narrows the run down to a single case, because every test is
registered under its `<binary>.<case>.noArgs` name:

```
# from the project build directory

ctest -R UTEST_IADataExportersFacade.create_default_lib_success
```

Add `--output-on-failure` to see the Google Test report of whatever failed.

The [debug-test.sh](/scripts/build/debug-test.sh) script of the [Quick build scripts](/doc/sections/en_US/5-project-build/5-38-quick-build-scripts.md) section performs the very same `ctest` call inside the `build/debug` directory, either on its own or as a step of the `Debug` build scripts, whenever the `--test` parameter is given (GNU/Linux based):

```
# inside the project root directory

scripts/build/debug-test.sh --test
```
