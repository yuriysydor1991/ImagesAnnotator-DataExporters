## Enabling the libcurl

[libcurl](https://curl.se/libcurl/) is a mandatory dependency of this library:
the image loader of the exporters preloads the web hosted image records through
it. The `ENABLE_LIBCURL` CMake option therefore defaults to `ON` and there is
nothing to switch on for a normal build.

```
# Inside the source root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/data-drivers-install-prefix
```

Setting `ENABLE_LIBCURL=OFF` makes both the enabler and the linker module
return early. The library will not compile in that state, so the option is only
useful when experimenting with the build system.

The libcurl integration is split into two CMake modules:

- [cmake/enablers/template-project-libcurl-enabler.cmake](/cmake/enablers/template-project-libcurl-enabler.cmake)
  only makes the dependency available. It runs before `src/` is added, so the
  unit test executables declared inside the source tree can already link
  `CURL::libcurl` themselves.
- [cmake/enablers/template-project-libcurl-linker.cmake](/cmake/enablers/template-project-libcurl-linker.cmake)
  runs after `src/` and links libcurl against the library target, which only
  exists at that point. The link is `PRIVATE`: no installable public header
  exposes a libcurl type, so the dependency must not leak into the exported
  interface of the library.

The enabler first probes the system wide libcurl with `find_package`. When
nothing is found it falls back to fetching the sources with `FetchContent` from
the repository and tag held in the `TEMPLATE_APP_CURL_GIT` and
`TEMPLATE_APP_CURL_GIT_TAG` cache variables:

```
# Inside the source root directory

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/data-drivers-install-prefix \
  -DTEMPLATE_APP_CURL_GIT_TAG=curl-8_11_0
```

Installing the system development package (for example `libcurl4-openssl-dev`
on Debian based distributions) is the faster route and keeps the configure
stage offline.
