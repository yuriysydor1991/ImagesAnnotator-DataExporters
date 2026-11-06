### Запобігання використання GTest з ОС

Для вимкнення використання ресурсів наявного фреймворку GTest необхідно встановити відповідне значення для CMake-змінної `IADE_GTEST_TRY_SYSTEM_PROBE` за допомогою команди конфігурування проекту (для GNU/Linux):

```
# з кореневої директорії проекту-шаблону

mkdir -vp build && cd build && cmake ../ -DIADE_ENABLE_UNIT_TESTS=ON -DIADE_GTEST_TRY_SYSTEM_PROBE=OFF && cmake --build . --target all
```

Під час виконання команди система побудови проекту спробує встановити GTest тільки у межах поточного проекту через мережу Інтернет з версією вказаною у файлі `cmake/imagesannotator-dataexporters-GTest-enabler.cmake`.
