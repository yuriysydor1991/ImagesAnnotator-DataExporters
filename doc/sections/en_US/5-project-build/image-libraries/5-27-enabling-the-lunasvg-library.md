## Enabling the lunasvg library (SVG)

In order to enable the [lunasvg](https://github.com/sammycage/lunasvg) library (the SVG vector image format) for the project by using the installed system one or by fetching it through the available connection set an `ON` value to the `ENABLE_LUNASVG` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_LUNASVG=ON
```

The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-lunasvg-enabler.cmake](/cmake/enablers/images/template-project-lunasvg-enabler.cmake) module for the targets (`lunasvg::lunasvg` for a system install, `lunasvg` for the FetchContent build) to link to your targets of interest.
