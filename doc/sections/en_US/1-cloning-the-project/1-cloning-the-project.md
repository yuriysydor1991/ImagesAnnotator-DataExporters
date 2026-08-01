# Cloning the project

The ImagesAnnotator dataset exporters library lives in a single Git repository. Clone it into your local directory by executing the next command in the terminal (GNU/Linux based):

```
git clone https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters.git
```

After this command executes you should find a directory named `ImagesAnnotator-DataExporters` inside your current one, so enter it by calling `cd ImagesAnnotator-DataExporters`.

The clone contains the whole project: the sources under [/src](/src), the installable public headers under [/src/lib/facade/public](/src/lib/facade/public), the CMake modules under [/cmake](/cmake) and this documentation under [/doc](/doc). The build produces the `libImagesAnnotatorDataExporters-0.11.so` shared library and, when the tests are enabled, the test binaries. The project is a library and builds no executable of its own.

The three exporters were taken out of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) application (its `src/annotator-business/exporters` component) so that the application and any other tool may produce the same training datasets without duplicating the code.

The library does not define annotation records of its own - it reads the ones provided by the sibling [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git) library, which has to be built and installed before this project may be configured. Examine the [Requirements](/doc/sections/en_US/3-requirements/3-requirements.md) section for the complete list of the needed tools and libraries, and the [The data drivers dependency](/doc/sections/en_US/5-project-build/5-36-the-data-drivers-dependency.md) subsection for the way to point CMake at the installed data drivers.
