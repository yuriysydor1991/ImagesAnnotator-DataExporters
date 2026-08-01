# Клонування проекту

Бібліотека експортерів наборів даних ImagesAnnotator розміщена в єдиному Git-репозиторії. Склонуй його у свою локальну директорію, виконавши наступну команду в терміналі (для GNU/Linux):

```
git clone https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters.git
```

Після успішного виконання даної команди у поточній директорії повинна з'явитись нова піддиректорія `ImagesAnnotator-DataExporters`, отож відкрий її за допомогою команди `cd ImagesAnnotator-DataExporters`.

Клон містить увесь проект: джерельні коди у [/src](/src), публічні заголовкові файли, які встановлюються, у [/src/lib/facade/public](/src/lib/facade/public), CMake-модулі у [/cmake](/cmake) і дану документацію у [/doc](/doc). Побудова створює динамічну бібліотеку `libImagesAnnotatorDataExporters-0.11.so` і, коли увімкнені тести, тестові виконувані файли. Проект є бібліотекою і не будує власного виконуваного файлу.

Три експортери були винесені з програми [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator.git) (з її компоненти `src/annotator-business/exporters`), щоб і сама програма, і будь-який інший інструмент могли створювати ті самі навчальні набори даних без дублювання коду.

Бібліотека не визначає власних записів анотацій - вона читає ті, які надає споріднена бібліотека [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git), яку необхідно побудувати і встановити перед конфігуруванням даного проекту. Переглянь секцію [Вимоги](/doc/sections/uk_UA/3-requirements/3-requirements.md) для повного списку необхідних інструментів і бібліотек, а також підсекцію [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md), щоб дізнатись, як вказати CMake шлях до встановлених драйверів даних.
