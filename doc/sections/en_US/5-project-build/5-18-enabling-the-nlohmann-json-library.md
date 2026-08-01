## Enabling the nlohmann JSON library

The [nlohmann](https://github.com/nlohmann/json.git) JSON library is an
optional enabler of the build system. This library does not use it: none of the
exporters and none of the installable public headers include it, and the
`ENABLE_NLOHMANN_JSON` option defaults to `OFF`.

The enabler is kept because the build system is shared with the sibling
projects. To switch it on - by using the system wide installed library or by
fetching it over the available connection - set the option to `ON`:

```
# Inside the source root directory

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/data-drivers-install-prefix \
  -DENABLE_NLOHMANN_JSON=ON
```

With the option on,
[cmake/enablers/template-project-nlohmann-json-enabler.cmake](/cmake/enablers/template-project-nlohmann-json-enabler.cmake)
probes the system package first and falls back to `FetchContent` with the
`TEMPLATE_APP_NLOHMANN_GIT` and `TEMPLATE_APP_NLOHMANN_GIT_TAG` cache
variables, then links `nlohmann_json::nlohmann_json` against the library
target.
