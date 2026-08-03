## Required tools for the GNU/Linux based OS

In order to build the library install the GCC C++ compiler with CMake and Git, plus the libcurl development files:

```
sudo apt install -y git g++ cmake libcurl4-openssl-dev
```

The project requires CMake `3.13` or newer and a compiler with the C++ `17` support.

The libcurl development package is not strictly required if the machine has Internet access: when no system wide libcurl is found the build fetches and builds it by itself. Examine the [Enabling the libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md) subsection.

OpenCV is optional. Installing its development package

```
sudo apt install -y libopencv-dev
```

gets the library an image cropper of its own, so that a consuming project needs no imaging stack to run the PyTorch Vision export. Without it everything else builds exactly the same, and the export keeps asking its consumer for a cropper. Examine the [Enabling the OpenCV image cropper](/doc/sections/en_US/5-project-build/5-37-enabling-the-OpenCV-image-cropper.md) subsection.

The second mandatory dependency, the ImagesAnnotatorDataDrivers library, is a sibling project of this one and is built from its own sources at [https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git). Build and install it first, then pass its install prefix to this project:

```
# from the project root

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers install prefix>
cmake --build build -j$(nproc)
```

Examine the [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md) subsection for the details, including the `IMAGES_ANNOTATOR_DATA_DRIVERS_PACKAGE` variable that overrides the searched package name.
