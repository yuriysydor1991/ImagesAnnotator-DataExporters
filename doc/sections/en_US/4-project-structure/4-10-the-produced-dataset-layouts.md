## The produced dataset layouts

Each `LibraryContext` descendant is implemented by one exporter class under [src/exporters](/src/exporters). This subsection describes what every one of them writes into the directory named by `LibraryContext::set_export_path()`. The interface that drives them is described in the [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection.

### What they all have in common

- The rectangle geometry is read from the `ImageRecordRect` fields `name`, `x`, `y`, `width` and `height`, always in the image own pixel coordinates. The image dimensions come from the `ImageRecord` `iwidth` and `iheight` fields.
- An image record holding no rectangles is skipped by every format.
- Before an image is touched the record is handed to the internal image loader. A record that points at a web page is downloaded with [libcurl](/doc/sections/en_US/5-project-build/5-14-enabling-libcurl.md) into a temporary preloads cache first, and from that moment on its `ImageRecord::get_full_path()` yields the local cached copy. A record that already points at a local file is left alone.
- The library decodes no image format itself. It copies image files as they are, or asks the supplied `IImageCropperFacility` to produce the cropped ones.
- A record that cannot be processed is logged and skipped, the run itself carries on.
- The YOLO v4, the three Ultralytics YOLO, the COCO and the Pascal VOC exporters create their destination directory. For the plain text and the PyTorch Vision formats `export_path` has to exist before `export_db()` is called.

The examples below all describe the very same two record database:

| Image | Size | Rectangles |
| --- | --- | --- |
| `/home/user/images/street.png` | 640 x 400 | `dog` (50, 20, 100, 40), `dog` (300, 25, 90, 45) |
| `/home/user/images/park.jpg` | 640 x 480 | `cat` (200, 130, 48, 52), `dog` (12, 8, 64, 64) |

### PlainTxtExportLibraryContext

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

### Yolo4ExportLibraryContext

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

`cfg/yolov4-obj.cfg` is the whole neural network descriptor of the YOLO v4 detector - the CSPDarknet53 backbone, the SPP and PANet neck and the three detection heads, the 162 layers of the `cfg/yolov4-custom.cfg` network of the [darknet](https://github.com/yuriysydor1991/darknetxx) project itself. Nothing in it has to be edited by hand before a training run, since every value that depends on the exported project is already filled in:

| Value | What the export writes |
| --- | --- |
| `classes` of each of the three `[yolo]` layers | the annotation names count of the database |
| `filters` of the convolution in front of each `[yolo]` layer | `(classes + 5) * 3` - the four box coordinates, the objectness and the classes, per anchor |
| `max_batches` | 2000 iterations per class, never fewer than 6000 and never fewer than the images the export has copied |
| `steps` | the 80% and the 90% of `max_batches` |
| `width` and `height` | 832 by 832 - a large input, so that the small objects of a big photo still reach the detectors |
| `subdivisions` | 32, the batch of 64 images taken two at a time |

The head of the file, for the two classes of this example:

```
[net]
# Testing
#batch=1
#subdivisions=1
# Training
batch=64
subdivisions=32
width=832
height=832
channels=3
...
max_batches=6000
policy=steps
steps=4800,5400
```

and the tail of every one of its three detectors:

```
[convolutional]
filters=21
size=1
stride=1
pad=1
activation=linear

[yolo]
mask=0,1,2
anchors=12,16, 19,36, 40,28, 36,75, 76,55, 72,146, 142,110, 192,243, 459,401
classes=2
num=9
...
```

The training run takes the exported directory as it is, plus the pre-trained backbone weights [yolov4.conv.137](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.conv.137):

```
darknet detector train data/obj.data cfg/yolov4-obj.cfg yolov4.conv.137 -map
```

Raise `subdivisions`, or lower `width` and `height` to another multiple of 32, when a training run goes out of the video memory. The anchors are the ones the YOLO v4 authors have got on the COCO dataset; `darknet detector calc_anchors` fits them to the dataset at hand.

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
1 0.539062 0.11875 0.140625 0.1125
```

and `data/park.txt` reads:

```
0 0.35 0.325 0.075 0.108333
1 0.06875 0.0833333 0.1 0.133333
```

Besides the records without rectangles, this exporter also skips the records whose `iwidth` or `iheight` is zero (the normalisation would divide by zero), and the ones whose image file is not a readable regular file or could not be copied. The `backup/` directory is created empty, darknet writes its weight snapshots there during the training.

### What the three Ultralytics YOLO layouts share

Three of the contexts write the layout every Ultralytics release reads - the one YOLO v5 introduced and v8, v11 and the ones after them kept. The directory is the same for all three of them, and so is the `data.yaml` descriptor. What the trained task changes is the single label file line of a rectangle, which is the only thing the three sections below differ in.

The exporter creates the export directory itself when it is not there yet, together with the two sub-directories:

```
export_path/
|-- data.yaml
|-- images/
|   `-- train/
|       |-- park.jpg
|       `-- street.png
`-- labels/
    `-- train/
        |-- park.txt
        `-- street.txt
```

An image and its annotations are paired by the path: a training run takes the image path, swaps the `images` path element for `labels` and the extension for `.txt`, and reads the annotations of that image there. Every annotated image file is copied into `images/train`. When a file of that name is already there, the copy receives a `-1`, `-2`, ... suffix before its extension - so `street.png` of a second source directory lands as `street-1.png` - and its label file follows that new name.

`data.yaml` is the whole descriptor of the dataset. The class names live in it, so this layout has no `obj.names` file of the darknet one:

```yaml
# The Ultralytics YOLO dataset descriptor, written by the ImagesAnnotator
# annotations dataset exporters library.
#
# Drop the path line below to move this directory elsewhere: an Ultralytics
# release then takes the directory holding this very file as the dataset root.
path: '/home/user/dataset'
train: images/train
val: images/train

names:
  0: 'cat'
  1: 'dog'
```

- `path` is the export directory, written out absolute, so that the descriptor resolves whatever the working directory of the training run is and wherever the framework keeps its own datasets directory. Dropping the line, as its comment says, makes the directory relocatable: an Ultralytics release then falls back to the directory holding the descriptor.
- `names` maps a class index onto an annotation name. The indexes are the positions in the sorted set of names the database reports, exactly the ones the label files carry. Every name is written in the YAML single quoted style, so that a colon, a hash or a quote inside an annotation name stays a part of the name instead of turning into syntax.
- `train` and `val` both name the one and only `images/train` directory. The whole set is offered for the validation as well, exactly as the darknet layout writes one and the same list into `train.txt` and `val.txt`: splitting it into a real training and a real validation part is left to the consumer.

A training run reads the exported directory as it is:

```
yolo detect train data=/home/user/dataset/data.yaml model=yolo11n.pt
```

with `detect` replaced by `obb` or `segment` for the two other layouts, and the model by the pre-trained weights of that very task.

All three normalise a rectangle into the `0..1` range against the record `iwidth` and `iheight`, and all three apply two guards the darknet exporter does not:

- **A rectangle reaching over an image edge is cut down to the image.** An Ultralytics release refuses a whole image over a single label coordinate outside of the `0..1` range, so a rectangle drawn a little over the border would otherwise have cost that image its place in the dataset.
- **A rectangle drawn from the right or from the bottom carries a negative width or height.** Its edges are sorted before it is cut, so it describes the same area as the one drawn the other way round.

A rectangle left with no area inside the image at all - one drawn entirely outside of it - is logged and dropped, while the image and the rest of its rectangles are exported as usual. Records without rectangles, and records with a zero `iwidth` or `iheight`, are skipped the way every format skips them.

### UltralyticsDetectExportLibraryContext

The detection dataset. Every rectangle becomes one line of the class index and the box:

```
<class index> <centre x> <centre y> <width> <height>
```

These are the very four normalised numbers the darknet layout writes into its own label files. So `labels/train/street.txt` reads:

```
1 0.15625 0.1 0.15625 0.1
1 0.539062 0.11875 0.140625 0.1125
```

and `labels/train/park.txt` reads:

```
0 0.35 0.325 0.075 0.108333
1 0.06875 0.0833333 0.1 0.133333
```

### UltralyticsObbExportLibraryContext

The oriented bounding box dataset. Every rectangle becomes one line of the class index and the four corners of the box, clockwise from the top left one:

```
<class index> <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>
```

A training run turns those eight numbers into the centre, size and rotation angle form of its own. `labels/train/street.txt` reads:

```
1 0.078125 0.05 0.234375 0.05 0.234375 0.15 0.078125 0.15
1 0.46875 0.0625 0.609375 0.0625 0.609375 0.175 0.46875 0.175
```

and `labels/train/park.txt` reads:

```
0 0.3125 0.270833 0.3875 0.270833 0.3875 0.379167 0.3125 0.379167
1 0.01875 0.0166667 0.11875 0.0166667 0.11875 0.15 0.01875 0.15
```

The annotations database knows axis aligned rectangles only, so every box written here carries the rotation angle of zero. That is not a wasted export: an OBB model trained on it detects the rotated instances of the very same objects.

### UltralyticsSegmentExportLibraryContext

The instance segmentation dataset. Every rectangle becomes one line of the class index and the points of the polygon which outlines the object:

```
<class index> <x1> <y1> ... <xn> <yn>
```

The format takes a polygon of any three or more points. The mask of a rectangle annotation is the rectangle outline itself, so the polygon written is its four corners - which makes these label files identical to the oriented bounding box ones above, both spelling out the corners of one and the same rectangle. What tells the two layouts apart is the training task that reads them: a polygon of an arbitrary point count here against the four corners of a rotated box there.

A mask is therefore only ever as tight as the drawn rectangle, and the trained segmentation model reproduces exactly that coarseness. Reach for this layout when a rectangle outline is mask enough, and annotate with real polygons in a tool that draws them when it is not.

### The YOLO formats this library does not write

Two more members of the YOLO format family are left out on purpose:

- **The pose, or keypoint, layout.** Its label line carries the keypoints of the object after the box, and the descriptor names their count and their left-right mirror pairs in a `kpt_shape` and a `flip_idx` key. The annotations database holds named rectangles and no keypoints at all, so there is nothing to write those columns out of.
- **The classification layout**, which is a `train/<annotation name>/` and a `val/<annotation name>/` directory of cropped images. That is the PyTorch Vision `ImageFolder` layout described below with one split directory added on top of it, and the split is the very thing this library leaves to its consumer.

### CocoExportLibraryContext

The COCO object detection dataset: the copied images and the single JSON descriptor over them. The exporter creates the export directory itself when it is not there yet, together with the two sub-directories:

```
export_path/
|-- annotations/
|   `-- instances_default.json
`-- images/
    |-- park.jpg
    `-- street.png
```

This is the one layout of the library read by something other than a YOLO training run. Detectron2, MMDetection, torchvision, the HuggingFace detection transformers, CVAT, FiftyOne, Label Studio and Roboflow all take the format, and none of them needs more than the descriptor and the directory holding the images:

```python
# Detectron2
from detectron2.data.datasets import register_coco_instances

register_coco_instances("my_dataset", {},
                        "/home/user/dataset/annotations/instances_default.json",
                        "/home/user/dataset/images")

# torchvision
from torchvision.datasets import CocoDetection

data = CocoDetection(root="/home/user/dataset/images",
                     annFile="/home/user/dataset/annotations/instances_default.json")
```

Every annotated image file is copied into `images/`. When a file of that name is already there, the copy receives a `-1`, `-2`, ... suffix before its extension, so `street.png` of a second source directory lands as `street-1.png`, and the descriptor names it under that new name.

`annotations/instances_default.json` is the whole descriptor. Its five keys are the ones the format defines, and for the two record database above it reads:

```json
{
"info": {"description": "The ImagesAnnotator annotations dataset", "version": "0.11.0"},
"licenses": [],
"images": [
  {"id": 1, "file_name": "street.png", "width": 640, "height": 400},
  {"id": 2, "file_name": "park.jpg", "width": 640, "height": 480}
],
"annotations": [
  {"id": 1, "image_id": 1, "category_id": 2, "bbox": [50, 20, 100, 40], "area": 4000, "iscrowd": 0, "segmentation": []},
  {"id": 2, "image_id": 1, "category_id": 2, "bbox": [300, 25, 90, 45], "area": 4050, "iscrowd": 0, "segmentation": []},
  {"id": 3, "image_id": 2, "category_id": 1, "bbox": [200, 130, 48, 52], "area": 2496, "iscrowd": 0, "segmentation": []},
  {"id": 4, "image_id": 2, "category_id": 2, "bbox": [12, 8, 64, 64], "area": 4096, "iscrowd": 0, "segmentation": []}
],
"categories": [
  {"id": 1, "name": "cat", "supercategory": ""},
  {"id": 2, "name": "dog", "supercategory": ""}
]
}
```

- **`bbox`** is `[x, y, width, height]` of the top left corner, in the image own pixels. These are the very four numbers of the `ImageRecordRect`, which makes this the one layout of the library that normalises nothing away: a YOLO label file cannot be read back into pixels without the image size, this descriptor carries both.
- **`file_name`** is the file name alone, since the `images` directory itself is what a reader is handed as the dataset root. The `width` and `height` next to it are the record `iwidth` and `iheight`.
- **The identifiers** of all three arrays are running counters over what the export has really written out, and they start at `1`: a reader treats the category `0` as the background one. A category id is therefore the position of the name in the sorted set the database reports, plus one - which is the darknet and Ultralytics class index of that same name, plus one.
- **`area`** is the `width` multiplied by the `height` of the box, the value the evaluation of a trained detector splits its results by the object size with.
- **`iscrowd`** is `0` for every rectangle: the flag marks a single annotation covering a whole crowd of objects, which a named rectangle of the annotations database never is.
- **`segmentation`** is left empty, since a rectangle carries no mask. That is what makes the export a detection dataset and not a segmentation one, and every reader of the format treats the empty list as "no mask here". Reach for `UltralyticsSegmentExportLibraryContext` when a box shaped mask is mask enough.
- An annotation name and a file name are both user text, so every one of them is written out JSON escaped - a quote, a backslash or a control character inside a name stays a part of the name instead of ending the string.

The two guards of the Ultralytics exporters apply here as well, for a different reason: a `bbox` reaching over an image edge would make the `area` written next to it, and every overlap ever computed against it, a lie.

- **A rectangle reaching over an image edge is cut down to the image.**
- **A rectangle drawn from the right or from the bottom carries a negative width or height.** Its edges are sorted before it is cut, so it describes the same area as the one drawn the other way round.

A rectangle left with no area inside the image at all is logged and dropped, while the image and the rest of its rectangles are exported as usual. Records without rectangles, and records with a zero `iwidth` or `iheight`, are skipped the way every format skips them - and, since the arrays are written after the images are copied, such a record takes no image id with it.

The whole set is offered as one dataset. Splitting it into a real training and validation part is left to the consumer, exactly as it is in every other layout here.

### PascalVocExportLibraryContext

The Pascal VOC dataset, in the devkit directory shape: the copied images, one XML descriptor per image over them and the image lists naming those. The exporter creates the export directory itself when it is not there yet, together with the three sub-directories:

```
export_path/
|-- Annotations/
|   |-- park.xml
|   `-- street.xml
|-- ImageSets/
|   `-- Main/
|       |-- train.txt
|       `-- val.txt
`-- JPEGImages/
    |-- park.jpg
    `-- street.png
```

These are the very files [LabelImg](https://github.com/HumanSignal/labelImg) saves its own work in, so an exported project opens for correction in that annotator directly: point it at `JPEGImages` with `Annotations` as its save directory and every rectangle of the project is there to be edited and written back. Every reader of the format takes the same three directories - the MMDetection `XMLDataset` with its default `img_subdir` and `ann_subdir`, and the torchvision `VOCDetection`:

```python
# MMDetection
dict(type='XMLDataset',
     data_root='/home/user/dataset/',
     ann_file='ImageSets/Main/train.txt',
     data_prefix=dict(sub_data_root=''),
     metainfo=dict(classes=('cat', 'dog')))

# torchvision, which joins the VOCdevkit/VOC2012 path of its year onto the root
# it is given, so the export directory has to be placed under that name
from torchvision.datasets import VOCDetection

data = VOCDetection(root="/home/user", year="2012", image_set="train")
```

Every annotated image file is copied into `JPEGImages/`. When a file of that name is already there, the copy receives a `-1`, `-2`, ... suffix before its extension, so `street.png` of a second source directory lands as `street-1.png`, and its descriptor follows that new name. The suffix is given for a taken file **stem** and not only for a taken file name, because this is the one layout that keys the descriptor to the stem: `park.jpg` and `park.png` would otherwise both ask for `Annotations/park.xml`.

`Annotations/street.xml` holds the whole description of one image and its rectangles:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<annotation>
  <folder>JPEGImages</folder>
  <filename>street.png</filename>
  <source>
    <database>The ImagesAnnotator annotations dataset</database>
  </source>
  <size>
    <width>640</width>
    <height>400</height>
    <depth>3</depth>
  </size>
  <segmented>0</segmented>
  <object>
    <name>dog</name>
    <pose>Unspecified</pose>
    <truncated>0</truncated>
    <difficult>0</difficult>
    <bndbox>
      <xmin>50</xmin>
      <ymin>20</ymin>
      <xmax>150</xmax>
      <ymax>60</ymax>
    </bndbox>
  </object>
  <object>
    <name>dog</name>
    <pose>Unspecified</pose>
    <truncated>0</truncated>
    <difficult>0</difficult>
    <bndbox>
      <xmin>300</xmin>
      <ymin>25</ymin>
      <xmax>390</xmax>
      <ymax>70</ymax>
    </bndbox>
  </object>
</annotation>
```

- **`bndbox`** is the two corner points the rectangle was drawn between, in the image own pixels: `xmin`/`ymin` is its origin and `xmax`/`ymax` that origin plus its size. LabelImg turns that pair back into the very same rectangle, which is what makes the migration a two way one. The 1-based coordinates the original VOC devkit used are not written: a reader that expects them - the MMDetection `XMLDataset` subtracts one from all four by default - shifts the box by a single pixel and keeps its size, since both corners move together.
- **`name`** is the annotation name, written out as it stands. This is the one layout of the library that numbers nothing, so the position of a name in the sorted set the database reports decides nothing here, and adding a name to a project renumbers no file that was already exported.
- **`folder`** and **`filename`** are the directory holding the image and its file name, which a reader joins onto the dataset root it was handed. The `width` and `height` of `size` are the record `iwidth` and `iheight`.
- **`depth`** is `3` for every image. An `ImageRecord` carries no channel count, and the format leaves no way to write the size without one.
- **`truncated`** is `1` exactly when the image edge really did cut the rectangle down, which is what the flag means: the object continues past the picture. A rectangle drawn wholly inside the image gets a `0`.
- **`difficult`** is `0` for every rectangle - the database has no such mark, and the standard VOC evaluation drops a `1` from its results altogether. **`pose`** is `Unspecified`, since no viewing angle is held either.
- **`segmented`** is `0`: the flag promises a mask in the segmentation directories of the devkit, and a rectangle carries none. Reach for `UltralyticsSegmentExportLibraryContext` when a box shaped mask is mask enough.
- An annotation name and a file name are both user text, so both are written out XML escaped - an `&`, a `<` or a `>` inside a name stays a part of the name instead of opening markup. The control characters XML 1.0 carries no way to write at all, not even as a numeric reference, are dropped from a name rather than written into a file no parser would accept.

`ImageSets/Main/train.txt` and `ImageSets/Main/val.txt` both list the stems of the copied images, one per line:

```
street
park
```

The two files are written with identical content, exactly as the darknet layout writes one and the same list into its own `train.txt` and `val.txt`: splitting the set into a real training and validation part is left to the consumer. Their names are not free, though - a reader of this format asks for a split by name, and `train`, `val` and `trainval` are the ones it accepts.

The two guards of the Ultralytics exporters apply here as well:

- **A rectangle reaching over an image edge is cut down to the image**, since a `bndbox` naming pixels the image does not have is a box no reader can crop to. What the cut leaves behind is marked with the `truncated` flag above.
- **A rectangle drawn from the right or from the bottom carries a negative width or height.** Its edges are sorted before it is cut, so it describes the same area as the one drawn the other way round, and sorting alone never sets `truncated`.

A rectangle left with no area inside the image at all is logged and dropped, while the image and the rest of its rectangles are exported as usual - an image whose every rectangle was dropped still reaches the lists, with a descriptor carrying no `object` element. Records without rectangles, and records with a zero `iwidth` or `iheight`, are skipped the way every format skips them.

One limitation belongs to a reader rather than to the export: the MMDetection `XMLDataset` builds an image path by appending `.jpg` to a name of the list, so a project of PNG images is read by it only after that class is told otherwise. The descriptors themselves name the real file with their `filename` element, and every other reader of the format uses it.

### PyTorchExportLibraryContext

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

This is the only format that needs a cropper from `PyTorchExportLibraryContext::set_cropper()`: without one `export_db()` logs the fact and returns `false` right away. For every rectangle the exporter calls

```cpp
cropper->crop_out_2_fs(ir, irr, tofpath);
```

with `tofpath` set to `<export_path>/<annotation name>/<image file name>`. The implementation is free to rewrite that path, for example to append the extension of the format it encodes to. Rectangles with an empty annotation name are skipped.

The library performs no image decoding here at all - the cropped file is entirely the work of the supplied cropper, and its return value decides whether the rectangle counts as exported. Note that all the rectangles of one annotation name inside one image are offered the very same destination path, so a cropper that has to keep them apart has to make the path unique itself. See the [The dataset exporters API](/doc/sections/en_US/4-project-structure/4-9-the-dataset-exporters-api.md) subsection for an implementation sketch.
