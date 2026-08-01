# Project build

The project is a CMake library project: it produces the shared object
`libImagesAnnotatorDataExporters-0.11.so` together with its installable public
headers and its CMake package. No application binary is built, only the library
and its test executables.

Before the first configure make sure the mandatory
[ImagesAnnotatorDataDrivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md)
is installed and reachable: every configure command in the sections below needs
a `-DCMAKE_PREFIX_PATH` pointing at its install prefix.

1. [IDE build](/doc/sections/en_US/5-project-build/5-1-IDE-build.md)
1. [Command line build](/doc/sections/en_US/5-project-build/5-2-command-line-build.md)
1. [The ImagesAnnotatorDataDrivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md)
1. Enabling testing
    1. [Enabling unit testing](/doc/sections/en_US/5-project-build/testing/5-3-1-enabling-unit-testing.md)
    1. [Disabling system GTest probe](/doc/sections/en_US/5-project-build/testing/5-3-2-disabling-system-GTest-probe.md)
1. [Documentation build](/doc/sections/en_US/5-project-build/documentation/5-4-documentation-build.md)
1. [Configuring the documentation install support](/doc/sections/en_US/5-project-build/documentation/5-5-configuring-the-documentation-install-support.md)
1. [Customizing the installable library name segments](/doc/sections/en_US/5-project-build/compression/5-23-customizing-library-name-segments.md)
1. Code quality & sanitizers
    1. [Enabling and performing code formatting target](/doc/sections/en_US/5-project-build/code-quality/5-6-enabling-and-performing-code-formatting-target.md)
    1. [Enabling the static code analyzer target with cppcheck](/doc/sections/en_US/5-project-build/code-quality/5-7-enabling-the-static-code-analyzer-target-with-cppcheck.md)
    1. [Enabling the static code analyzer with clang-tidy](/doc/sections/en_US/5-project-build/code-quality/5-8-enabling-static-code-analyzer-with-clang-tidy.md)
1. Containers / CI
    1. [Enabling Jenkins pipeline inside Docker container](/doc/sections/en_US/5-project-build/containers-ci/5-17-enabling-Jenkins-pipeline-inside-Docker-container.md)
1. Packagers
    1. [Enabling DEB package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-10-enabling-DEB-package-generation-with-cpack.md)
    1. [Enabling FreeBSD pkg package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-20-enabling-FreeBSD-pkg-package-generation-with-cpack.md)
    1. [Enabling WIX MSI package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-21-enabling-WIX-MSI-package-generation-with-cpack.md)
    1. [Enabling RPM package generation with cpack](/doc/sections/en_US/5-project-build/packagers/5-22-enabling-RPM-package-generation-with-cpack.md)
1. Libraries
    1. [Enabling the libcurl (mandatory)](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md)
    1. [Enabling the nlohmann JSON library](/doc/sections/en_US/5-project-build/5-18-enabling-the-nlohmann-json-library.md)
1. Security / Cryptography
    1. [Enabling the OpenSSL library](/doc/sections/en_US/5-project-build/security/5-35-enabling-the-openssl-library.md)
1. Compression
    1. [Enabling the zlib library](/doc/sections/en_US/5-project-build/compression/5-23-enabling-the-zlib-library.md)
    1. [Enabling the liblzma library](/doc/sections/en_US/5-project-build/compression/5-34-enabling-the-liblzma-library.md)
1. Images
    1. [Enabling the libpng library](/doc/sections/en_US/5-project-build/image-libraries/5-24-enabling-the-libpng-library.md)
    1. [Enabling the libjpeg library](/doc/sections/en_US/5-project-build/image-libraries/5-25-enabling-the-libjpeg-library.md)
    1. [Enabling the libwebp library](/doc/sections/en_US/5-project-build/image-libraries/5-26-enabling-the-libwebp-library.md)
    1. [Enabling the lunasvg library (SVG)](/doc/sections/en_US/5-project-build/image-libraries/5-27-enabling-the-lunasvg-library.md)
    1. [Enabling the giflib library (GIF)](/doc/sections/en_US/5-project-build/image-libraries/5-28-enabling-the-giflib-library.md)
    1. [Enabling the libtiff library (TIFF)](/doc/sections/en_US/5-project-build/image-libraries/5-29-enabling-the-libtiff-library.md)
    1. [Enabling the OpenEXR library (EXR / HDR)](/doc/sections/en_US/5-project-build/image-libraries/5-30-enabling-the-openexr-library.md)
    1. [Enabling the OpenJPEG library (JPEG 2000)](/doc/sections/en_US/5-project-build/image-libraries/5-31-enabling-the-openjpeg-library.md)
    1. [Enabling the libavif library (AVIF)](/doc/sections/en_US/5-project-build/image-libraries/5-32-enabling-the-libavif-library.md)
    1. [Enabling the libheif library (HEIF/HEIC)](/doc/sections/en_US/5-project-build/image-libraries/5-33-enabling-the-libheif-library.md)

Only the ImagesAnnotatorDataDrivers library and libcurl are required to build.
Every other entry of the `Libraries`, `Security`, `Compression` and `Images`
groups is an optional build system enabler that this library itself does not
use; they are kept because the build system is shared with the sibling
projects.
