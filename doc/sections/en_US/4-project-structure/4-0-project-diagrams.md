## Project diagrams

The class diagram of the `lib` component - the installable interface, the implementation hidden behind it and the records of the data drivers dependency it reads:

![lib component](/doc/diagrams/images/lib-class-structure.svg)

Three groups are drawn:

- `ImagesAnnotatorDataDrivers011` - the records the exporters consume (`IImagesPathsDBProvider`, `ImageRecord`), owned by the dependency;
- `ImagesAnnotatorDataExporters011` - everything installed from [src/lib/facade/public](/src/lib/facade/public): `LibraryFacade`, `ILib`, `LibraryContext` with its six layout descendants, `IExporter` and the consumer implemented `IImageCropperFacility`;
- the implementation, whose symbols stay inside the shared object: `LibMain`, `LibFactory`, the exporters - the three Ultralytics YOLO ones sharing the `Ultralytics2FolderExporter` base - `ImageLoader` and `CURLController`.

The image is rendered from the PlantUML source at [doc/diagrams/plantuml/lib-class-structure.puml](/doc/diagrams/plantuml/lib-class-structure.puml). Edit that file and render it again whenever the public interface changes:

```
# from the project root directory

plantuml -tsvg -o ../images doc/diagrams/plantuml/lib-class-structure.puml
```
