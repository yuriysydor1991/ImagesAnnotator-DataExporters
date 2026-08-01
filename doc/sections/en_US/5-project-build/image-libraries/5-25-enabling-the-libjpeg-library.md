## Enabling the libjpeg library

In order to enable the [libjpeg-turbo](https://libjpeg-turbo.org/) library (the JPG/JPEG image format) for the project by using the installed system one or by fetching it through the available connection set an `ON` value to the `ENABLE_LIBJPEG` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_LIBJPEG=ON
```

The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-libjpeg-enabler.cmake](/cmake/enablers/images/template-project-libjpeg-enabler.cmake) module for the targets (`JPEG::JPEG` for a system install, `jpeg-static`/`jpeg` or `turbojpeg` for the FetchContent build) to link to your targets of interest.
