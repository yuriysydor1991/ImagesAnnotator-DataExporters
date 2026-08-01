# What the build produces

There is no main executable in this project. A plain build creates a single
artefact, the shared library, plus the symbolic links that carry its version:

```
build/src/lib/libImagesAnnotatorDataExporters-0.11.so.0.11.0
build/src/lib/libImagesAnnotatorDataExporters-0.11.so.0
build/src/lib/libImagesAnnotatorDataExporters-0.11.so
```

The `.so.0.11.0` file is the real library (`VERSION 0.11.0`). The `.so.0` link
is the `SONAME` the dynamic linker resolves at run time (`SOVERSION 0`), and
the bare `.so` link is the one the compiler follows while linking your program.

Test binaries appear only if the project was configured with
[`ENABLE_UNIT_TESTS`](/doc/sections/en_US/5-project-build/testing/5-3-1-enabling-unit-testing.md)
and/or `ENABLE_COMPONENT_TESTS` switched on. The unit tests are named with the
`UTEST_` prefix and compile the library sources straight from the tree against
the gmock stand-ins, while the component tests are named with the `CTEST_`
prefix and link the produced shared library.

Everything else the library offers is reached from your own code through the
installable public headers. Continue with:

1. [Searching for the generated binaries](/doc/sections/en_US/6-run-the-executable/6-2-1-searching-for-the-generated-executable.md)
1. [Using the produced library](/doc/sections/en_US/6-run-the-executable/6-2-2-starting-the-generated-executable.md)
1. [Run tests by the ctest](/doc/sections/en_US/6-run-the-executable/6-3-1-run-tests-by-the-ctest.md)
1. [Manual tests run](/doc/sections/en_US/6-run-the-executable/6-3-2-manual-tests-run.md)
