## The produced dataset layouts

Each value of the `ExportFormat` enumeration is implemented by one exporter class under [src/lib/libmain/exporters](/src/lib/libmain/exporters). This subsection describes what every one of them writes into the `ExportContext::export_path` directory. The interface that drives them is described in the [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection.

### What all the three have in common

- The rectangle geometry is read from the `ImageRecordRect` fields `name`, `x`, `y`, `width` and `height`, always in the image own pixel coordinates. The image dimensions come from the `ImageRecord` `iwidth` and `iheight` fields.
- An image record holding no rectangles is skipped by every format.
- Before an image is touched the record is handed to the internal image loader. A record that points at a web page is downloaded with [libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md) into a temporary preloads cache first, and from that moment on its `ImageRecord::get_full_path()` yields the local cached copy. A record that already points at a local file is left alone.
- The library decodes no image format itself. It copies image files as they are, or asks the supplied `IImageCropperFacility` to produce the cropped ones.
- A record that cannot be processed is logged and skipped, the run itself carries on.
- Only the YOLO v4 exporter creates its destination directory. For the two other formats `export_path` has to exist before `export_db()` is called.

The examples below all describe the very same two record database:

| Image | Size | Rectangles |
| --- | --- | --- |
| `/home/user/images/street.png` | 640 x 400 | `dog` (50, 20, 100, 40), `dog` (300, 25, 90, 45) |
| `/home/user/images/park.jpg` | 640 x 480 | `cat` (200, 130, 48, 52), `dog` (12, 8, 64, 64) |

### ExportFormat::PlainTxt2Folder

The simplest layout: one plain text file per annotation name, written straight into the export directory and named `<annotation-name>.txt`.

```
export_path/
|-- cat.txt
`-- dog.txt
```

A file gets one line per image record that carries at least one rectangle of that name:

```
<image full path> <rect count> <x> <y> <width> <height> [<x> <y> <width> <height> ...]
```

The count is the number of rectangles of that annotation name found in that image, and exactly that many coordinate quadruples follow it. So `dog.txt` reads:

```
/home/user/images/street.png 2 50 20 100 40 300 25 90 45
/home/user/images/park.jpg 1 12 8 64 64
```

and `cat.txt` reads:

```
/home/user/images/park.jpg 1 200 130 48 52
```

The lines follow the order of the records in the database. No image file is copied or rewritten - the first field is the `ImageRecord::get_full_path()` value, which for a preloaded web hosted record is the path of its temporary cached copy. A text file of an already existing name is truncated as soon as the export run opens it.

### ExportFormat::Yolo42Folder

The darknet training directory of the YOLO v4 detector. The exporter creates the export directory itself when it is not there yet, together with the `data`, `cfg` and `backup` sub-directories:

```
export_path/
|-- backup/
|-- cfg/
|   `-- yolov4-obj.cfg
`-- data/
    |-- obj.data
    |-- obj.names
    |-- park.jpg
    |-- park.txt
    |-- street.png
    |-- street.txt
    |-- train.txt
    `-- val.txt
```

`data/obj.names` lists every distinct annotation name of the whole database, one per line. The list is the sorted set the database reports, so the line number of a name is also its class index - `cat` is the class `0` and `dog` is the class `1` here:

```
cat
dog
```

`data/obj.data` names the class count and the paths of the other files, all of them relative to the export directory:

```
classes = 2
train = data/train.txt
valid = data/val.txt
names = data/obj.names
backup = backup/
```

`cfg/yolov4-obj.cfg` carries the two values that depend on the class count, the filter count being `(classes + 5) * 3`:

```
classes = 2
filters = 21
```

Every annotated image file is copied into `data/`. When a file of that name is already there, the copy receives a `-1`, `-2`, ... suffix before its extension, so `street.png` of a second source directory lands as `street-1.png`.

`data/train.txt` and `data/val.txt` both list the copied images, one relative path per line. The two files are written with identical content, splitting the set into a real training and validation part is left to the consumer:

```
data/street.png
data/park.jpg
```

Every copied image is accompanied by a `data/<image stem>.txt` file holding one line per rectangle:

```
<class index> <centre x> <centre y> <width> <height>
```

The four numbers are normalised into the `0..1` range against the record `iwidth` and `iheight`, the centre being the rectangle origin plus half of its size. The lines follow the order of the rectangles inside the record. So `data/street.txt` reads:

```
1 0.15625 0.1 0.15625 0.1
1 0.539063 0.11875 0.140625 0.1125
```

and `data/park.txt` reads:

```
0 0.35 0.325 0.075 0.108333
1 0.06875 0.0833333 0.1 0.133333
```

Besides the records without rectangles, this exporter also skips the records whose `iwidth` or `iheight` is zero (the normalisation would divide by zero), and the ones whose image file is not a readable regular file or could not be copied. The `backup/` directory is created empty, darknet writes its weight snapshots there during the training.

### ExportFormat::PyTorchVisionFolder

The classification layout the PyTorch Vision `ImageFolder` dataset reads: one directory per annotation name, holding the images cropped down to the rectangles carrying that name.

```
export_path/
|-- cat/
|   `-- park.jpg
`-- dog/
    |-- park.jpg
    `-- street.png
```

The annotation name is used verbatim as the directory name, and the directory is created when it is not there yet. The export directory itself is not created, it has to exist beforehand.

This is the only format that needs an `ExportContext::cropper`: without one `export_db()` logs the fact and returns `false` right away. For every rectangle the exporter calls

```cpp
cropper->crop_out_2_fs(ir, irr, tofpath);
```

with `tofpath` set to `<export_path>/<annotation name>/<image file name>`. The implementation is free to rewrite that path, for example to append the extension of the format it encodes to. Rectangles with an empty annotation name are skipped.

The library performs no image decoding here at all - the cropped file is entirely the work of the supplied cropper, and its return value decides whether the rectangle counts as exported. Note that all the rectangles of one annotation name inside one image are offered the very same destination path, so a cropper that has to keep them apart has to make the path unique itself. See the [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection for an implementation sketch.
