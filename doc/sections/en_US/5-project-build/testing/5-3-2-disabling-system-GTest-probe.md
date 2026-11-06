### Disabling system GTest probe

By default the build system first probes the system wide installed GTest
framework and only falls back to fetching it over the Internet. To skip that
probe and always use the fetched copy set the `IADE_GTEST_TRY_SYSTEM_PROBE` CMake
variable to `OFF` (GNU/Linux based):

```
# from the project root

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/data-drivers-install-prefix \
  -DIADE_ENABLE_UNIT_TESTS=ON \
  -DIADE_GTEST_TRY_SYSTEM_PROBE=OFF
cmake --build build -j$(nproc)
```

With the probe disabled the build system makes GTest available through the
Internet for the current project only, at the version pinned in the
[cmake/enablers/imagesannotator-dataexporters-GTest-enabler.cmake](/cmake/enablers/imagesannotator-dataexporters-GTest-enabler.cmake)
file. The repository and the tag are held in the `IADE_GTEST_GIT` and
`IADE_GTEST_GIT_TAG` cache variables, so a different GTest revision can
be requested without editing the file:

```
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/data-drivers-install-prefix \
  -DIADE_ENABLE_UNIT_TESTS=ON \
  -DIADE_GTEST_TRY_SYSTEM_PROBE=OFF \
  -DIADE_GTEST_GIT_TAG=v1.15.2
```

The variable is only consulted when at least one of `IADE_ENABLE_UNIT_TESTS` and
`IADE_ENABLE_COMPONENT_TESTS` is `ON`; otherwise the GTest enabler returns
immediately and no test framework is looked up at all.
