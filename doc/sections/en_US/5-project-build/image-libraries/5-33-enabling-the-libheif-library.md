## Enabling the libheif library (HEIF/HEIC)

In order to enable the [libheif](https://github.com/strukturag/libheif) library (the HEIF/HEIC image format) for the project by using the installed system one or by fetching it through the available connection set an `ON` value to the `ENABLE_LIBHEIF` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_LIBHEIF=ON
```

A codec is required to actually decode HEIF/HEIC (e.g. libde265). The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-libheif-enabler.cmake](/cmake/enablers/images/template-project-libheif-enabler.cmake) module for the target (`heif`) to link to your targets of interest.
