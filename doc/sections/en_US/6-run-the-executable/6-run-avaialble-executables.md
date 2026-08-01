# Run available executables

This project is a library, not an application. The build produces no program
you can start: the only shipped binary is the shared object
`libImagesAnnotatorDataExporters-0.11.so`, which is meant to be linked into your
own code. The only runnable files it builds are the test binaries, and they are
built solely when the testing options are switched on.

1. [What the build produces](/doc/sections/en_US/6-run-the-executable/6-run-the-executable.md)
    1. [Searching for the generated binaries](/doc/sections/en_US/6-run-the-executable/6-2-1-searching-for-the-generated-executable.md)
    1. [Using the produced library](/doc/sections/en_US/6-run-the-executable/6-2-2-starting-the-generated-executable.md)
1. Tests run
    1. [Run tests by the ctest](/doc/sections/en_US/6-run-the-executable/6-3-1-run-tests-by-the-ctest.md)
    1. [Manual tests run](/doc/sections/en_US/6-run-the-executable/6-3-2-manual-tests-run.md)
