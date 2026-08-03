## Обов'язкові інструменти для ОС на базі GNU/Лінукс

Для того щоб побудувати бібліотеку, необхідно встановити компілятор GCC C++ разом з системою побудови CMake і системою версіювання Git, а також файли розробки libcurl:

```
sudo apt install -y git g++ cmake libcurl4-openssl-dev
```

Проект потребує CMake версії `3.13` або новішої і компілятора з підтримкою C++ `17`.

Пакунок розробки libcurl не є строго обов'язковим, якщо машина має доступ до мережі Інтернет: коли системної libcurl не знайдено, побудова завантажує і будує її самостійно. Переглянь підсекцію [Вмикання підтримки libcurl](/doc/sections/uk_UA/5-project-build/5-14-enabling-libcurl.md).

Друга обов'язкова залежність, бібліотека ImagesAnnotatorDataDrivers, є спорідненим проектом даного і будується зі своїх власних джерельних кодів за адресою [https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers.git). Спершу побудуй і встанови її, а тоді передай даному проекту її префікс встановлення:

```
# з кореневої директорії проекту

cmake -S . -B build -DCMAKE_PREFIX_PATH=<data drivers install prefix>
cmake --build build -j$(nproc)
```

Переглянь підсекцію [Залежність від бібліотеки драйверів даних](/doc/sections/uk_UA/5-project-build/5-36-the-data-drivers-dependency.md) для деталей, включно зі змінною `IMAGES_ANNOTATOR_DATA_DRIVERS_PACKAGE`, яка перевизначає ім'я пакунка, що шукається.
