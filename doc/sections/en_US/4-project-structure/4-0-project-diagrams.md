## Project diagrams

The class diagram of the `lib` component - the installable interface, the implementation hidden behind it and the records of the data drivers dependency it reads:

![lib component](/doc/diagrams/images/lib-class-structure.svg)

Three groups are drawn:

- `ImagesAnnotatorDataDrivers013` - the records the exporters consume (`IImagesPathsDBProvider`, `ImageRecord`), owned by the dependency;
- `ImagesAnnotatorDataExporters013` - everything installed from [src/lib/facade/public](/src/lib/facade/public): `IADataExportersFacade`, `IADataExportersLib`, `IADataExportersContext` with its thirteen layout descendants, `IExporter` and the consumer implemented `IImageCropperFacility`;
- the implementation, whose symbols stay inside the shared object: `LibMain`, `LibFactory`, the exporters - the three Ultralytics YOLO ones sharing the `Ultralytics2FolderExporter` base and the darknetxx `Dxxwjz2FolderExporter` extending the `Yolo42FolderExporter` one, the `Dxxwjz3FolderExporter` extending the `Dxxwjz2FolderExporter` one and the `Dxxwjz3pProjectExporter` extending the `Dxxwjz2pProjectExporter` one - the writers the YOLO v4 and the darknetxx exporters write their documents by (`Yolov4CfgWriter`, the `Dxxwjz2NetworkWriter` family and `Dxxwjz2pAnnotationsWriter`, with `Dxxwjz3CfgJson`), `ImageLoader`, `ZlibController` and `CURLController`.

The image is drawn by hand after the PlantUML source at [doc/diagrams/plantuml/lib-class-structure.puml](/doc/diagrams/plantuml/lib-class-structure.puml), which stays the complete description of the component: a fan of thirteen layouts laid out automatically comes out several thousand pixels wide, so the drawing condenses the edges every layout repeats into one each. Edit the source whenever the public interface changes and bring the drawing along with it. Rendering the source into a directory of its own shows what to compare the drawing against, without overwriting it:

```
# from the project root directory

plantuml -tsvg -o "$(pwd)/build/diagrams" doc/diagrams/plantuml/lib-class-structure.puml
```
