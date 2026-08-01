# Forking and replacing the origin

To continue the library development on your own, fork it first. Forking is usually performed in the Git hosting Web interface panel, on the [project page](https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters).

In order to copy the whole project into a new clean repository instead (**clean repository with no** `README.md`, `.gitignore`, `LICENSE` or any other files) clone the current project into your local directory and perform the origin replacement command from inside the project root directory:

```
# REPLACE next URL with yours
git remote set-url origin https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters.git
```

Replace the `https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters.git` URL with the one of your newly created repository.

To verify the successful repository remote URL (where all the changes will be pushed) replacement execute next command:

```
git remote -v
```

Which should show your new repository.

Finally push your repository to the new location by executing next command:

```
git push
```

To keep receiving the upstream changes after the replacement, add the original repository under a separate remote name:

```
git remote add upstream https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters.git
git fetch upstream
```

Keep in mind that the project name, the produced `libImagesAnnotatorDataExporters-0.11.so` binary name, the `ImagesAnnotatorDataExporters-0.11` CMake package and the `include/ImagesAnnotatorDataExporters-0.11/` header install directory are all derived from the CMake project name, so renaming the fork means renaming them as well - examine the [Changing the project and executable name](/doc/sections/en_US/4-project-structure/4-2-changing-the-project-and-executable-name.md) subsection.

The project is distributed under the "Simplified BSD License", so keep the copyright notice of the [/LICENSE](/LICENSE) file in the redistributed sources and binaries.
