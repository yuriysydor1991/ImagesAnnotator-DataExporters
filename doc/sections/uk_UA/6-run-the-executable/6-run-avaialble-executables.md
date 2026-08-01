# Запуск доступних виконуваних файлів

Даний проект є бібліотекою, а не програмою. Побудова не створює програми, яку
можна запустити: єдиним бінарним файлом, що постачається, є динамічна
бібліотека `libImagesAnnotatorDataExporters-0.11.so`, призначена для лінкування у
власний код розробника. Єдиними виконуваними файлами, які будуються, є тестові
бінарні файли, і вони будуються лише тоді, коли увімкнені опції тестування.

1. [Що створює побудова](/doc/sections/uk_UA/6-run-the-executable/6-run-the-executable.md)
    1. [Пошук згенерованих бінарних файлів](/doc/sections/uk_UA/6-run-the-executable/6-2-1-searching-for-the-generated-executable.md)
    1. [Використання створеної бібліотеки](/doc/sections/uk_UA/6-run-the-executable/6-2-2-starting-the-generated-executable.md)
1. Запуск тестів
    1. [Запуск тестів за допомогою ctest](/doc/sections/uk_UA/6-run-the-executable/6-3-1-run-tests-by-the-ctest.md)
    1. [Ручний запуск тестів](/doc/sections/uk_UA/6-run-the-executable/6-3-2-manual-tests-run.md)
