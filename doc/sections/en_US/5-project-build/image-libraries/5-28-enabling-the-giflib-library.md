## Enabling the giflib library (GIF)

In order to enable the [giflib](https://giflib.sourceforge.net/) library (the GIF image format) for the project set an `ON` value to the `ENABLE_GIFLIB` CMake variable.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers prefix> -DENABLE_GIFLIB=ON
```

giflib ships an autotools build only, so it is probed from the system exclusively (install `libgif-dev` on Debian/Ubuntu, `giflib-devel` on Fedora/openSUSE or `giflib` on Arch) - there is no FetchContent fallback. The exporters library decodes no images itself, so the enabler is kept available for the code built on top of this source tree. See the [cmake/enablers/images/template-project-giflib-enabler.cmake](/cmake/enablers/images/template-project-giflib-enabler.cmake) module for the target (`GIF::GIF`) to link to your targets of interest.
