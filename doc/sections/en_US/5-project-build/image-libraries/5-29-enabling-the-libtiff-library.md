## Enabling the libtiff library (TIFF)

In order to enable the [libtiff](http://www.libtiff.org/) library (the TIFF image format) for the project by using the installed system one or by fetching it through the available connection set an `ON` value to the `ENABLE_LIBTIFF` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_LIBTIFF=ON
```

The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-libtiff-enabler.cmake](/cmake/enablers/images/template-project-libtiff-enabler.cmake) module for the targets (`TIFF::TIFF` for a system install, `tiff` for the FetchContent build) to link to your targets of interest.
