## Extensions

The library has exactly two mandatory dependencies, both handled by a pair of CMake modules under [cmake/enablers](/cmake/enablers) - one making the dependency available before `src/` is added, so the test binaries may link it, and one linking it against the library target afterwards:

- ImagesAnnotatorDataDrivers, resolved by `find_package(... REQUIRED CONFIG)` and linked `PUBLIC`, because the installable headers name its record types. See [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md).
- libcurl, linked `PRIVATE`, used by `helpers::ImageLoader` to preload web hosted image records. `ENABLE_LIBCURL` is on by default. See [Enabling the libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md).

Everything else the build system offers is optional and off by default. The library itself uses none of it, but the modules are kept because they cost nothing while switched off and are useful when the project is extended:

- third party enablers for the nlohmann JSON library, zlib, liblzma, OpenSSL, libxml2, OpenCV and the image libraries (libpng, libjpeg, libwebp, lunasvg, giflib, libtiff, OpenEXR, OpenJPEG, libavif, libheif);
- code quality targets: clang-format, cppcheck, clang-tidy and the compiler sanitizers;
- package generation with CPack for DEB, RPM, FreeBSD pkg and WIX MSI;
- Doxygen documentation generation, optionally installed along with the library;
- a Jenkins pipeline that runs inside a Docker container.

Each of them is described in [Project build](/doc/sections/en_US/5-project-build/5-project-build.md).
