## Enabling the OpenCV image cropper

The library decodes no image format of its own. That is why `PyTorchExportLibraryContext::set_cropper()` exists: the [PyTorch Vision export](/doc/sections/en_US/4-project-structure/4-10-the-produced-dataset-layouts.md) has to cut the annotated rectangles out of the pictures, and it asks its consumer to do it over whatever imaging stack that project already links - the GTKmm, the Qt or the wxWidgets cutter of the ImagesAnnotator application, for instance.

A consumer that has no imaging stack to lend was stuck. So, when the build finds OpenCV, the library also ships a cropper of its own, and that consumer needs to supply nothing.

### The option

```
cmake -S . -B build -DENABLE_OPENCV=ON
```

`ENABLE_OPENCV` defaults to `ON`, and `ON` means *probe*, not *require*. The dependency is optional in both directions:

| Situation | Result |
| --- | --- |
| OpenCV is installed | the cropper is compiled in and the library links `core` and `imgcodecs` |
| OpenCV is not installed | the configure succeeds, prints why, and the library is built without the cropper |
| `-DENABLE_OPENCV=OFF` | the probe is skipped entirely, with the same result |

Nothing fails a configure over it. A missing OpenCV only means the export keeps asking its consumer for a cropper, exactly as it did before this one existed.

Point the probe at a prefix of your own with `-DOpenCV_DIR=<dir>` when the installation is not in a system default place, and narrow or widen the components it asks for with `-DTEMPLATE_APP_OPENCV_COMPONENTS="core;imgcodecs"`.

### Which cropper an export uses

The rule is one line: **a cropper you supply always wins.**

1. A cropper was handed over through `PyTorchExportLibraryContext::set_cropper()` - that cropper is used. A project that already decodes images its own way keeps doing exactly that, whether the library has OpenCV or not.
1. The cropper slot was left empty and the library has the OpenCV cropper - the export uses it, and you had to configure nothing.
1. The cropper slot was left empty and the library has no cropper - the export fails with a log line saying so, which is the behaviour that predates this feature.

So an existing consumer sees no change at all, and a new one may simply leave the field alone.

### Asking for it directly

`LibraryFacade::create_image_cropper()` hands out the same cropper, or a `nullptr` in a build without OpenCV. It is the way to find out whether a build has one:

```cpp
namespace iade = ImagesAnnotatorDataExporters011;

auto ctx = std::make_shared<iade::PyTorchExportLibraryContext>();
ctx->set_export_path("/tmp/pytorch-dataset");
ctx->set_db_provider(db);

// Optional. Leaving the field alone gets the very same cropper.
ctx->set_cropper(iade::LibraryFacade::create_image_cropper());

if (ctx->get_cropper() == nullptr) {
  // This build has no OpenCV - supply a cropper of your own, or pick an
  // export format that needs none.
}
```

### What it does

`crop_out_2_fs()` decodes the image named by the record, cuts the rectangle out and writes it as a PNG:

- a rectangle reaching over an edge is **clamped** to the image, so the overlapping part is written rather than the call failing - the same thing the application's own cutters do;
- a rectangle lying entirely outside the image fails the call;
- the destination path is rewritten to carry the `.png` extension, whatever it arrived with;
- an existing destination is never overwritten - a `-0`, `-1`, ... suffix is walked until a free name is found.

No OpenCV type appears in any installed header. The cropper is reached through the abstract `IImageCropperFacility` like any consumer supplied one, so **a project consuming this library needs no OpenCV of its own**, even when the library was built with it.

### Tests

`UTEST_OpenCVImageCropper` covers the cropper and is configured only in a build that found OpenCV, since every one of its cases drives it. The build without OpenCV is covered by `UTEST_LibFactory.create_image_cropper_matches_what_the_build_found`, which asserts that the factory hands out nothing there, and by `UTEST_PyTorchVisionFolderExporter.handles_a_missing_cropper_by_the_build`, which asserts the export fails without a cropper and succeeds with the built-in one.
