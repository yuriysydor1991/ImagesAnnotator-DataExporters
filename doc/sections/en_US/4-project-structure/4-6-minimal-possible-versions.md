## Minimal possible versions

The project deliberately declares the lowest build tool and language versions it can work with. The root [CMakeLists.txt](/CMakeLists.txt) asks for `cmake_minimum_required(VERSION 3.13)` and sets

```
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

C++17 is a floor rather than a preference: the exporters and the `helpers::ImageLoader` build their output paths with `std::filesystem`, and the installable headers use `std::shared_ptr` aliases and `enum class` only. Keeping the requirements low lets the library be configured, built and installed on older distributions and toolchains, and - being a library - it keeps the choice open for the projects that consume it, since a consumer may compile with any standard from C++17 upwards.

Nothing prevents raising the versions when a target environment calls for it: bump the `cmake_minimum_required(...)` value, set a newer `CMAKE_CXX_STANDARD` (for example `20` or `23`) or require newer versions of a dependency. Raising a version only narrows the set of systems able to build the library.

The one version that is not a free choice is the ImagesAnnotatorDataDrivers library: its records appear in the installable headers, so it and this library must be built with compatible compilers and standard library settings.
