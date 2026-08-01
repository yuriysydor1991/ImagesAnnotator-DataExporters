## Enabling the OpenEXR library (EXR / HDR)

In order to enable the [OpenEXR](https://openexr.com/) library (the EXR high dynamic range image format) for the project by using the installed system one or by fetching it through the available connection set an `ON` value to the `ENABLE_OPENEXR` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_OPENEXR=ON
```

The FetchContent build additionally pulls the Imath dependency automatically. The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-openexr-enabler.cmake](/cmake/enablers/images/template-project-openexr-enabler.cmake) module for the target (`OpenEXR::OpenEXR`) to link to your targets of interest.
