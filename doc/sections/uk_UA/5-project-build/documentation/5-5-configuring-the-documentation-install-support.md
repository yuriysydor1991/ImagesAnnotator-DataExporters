## Вмикання підтримки встановлення документації

Наявна можливість підтримки встановлення документації за допомогою встановлення змінних `IADE_ENABLE_DOC_DOXYGEN` і `IADE_DOXYGEN_DO_INSTALL` у значення `ON` під час конфігурування прокету-шаблону за допомогою команди:

```
# inside the project build directory

cmake ../ -DIADE_ENABLE_DOC_DOXYGEN=ON -DIADE_DOXYGEN_DO_INSTALL=ON
```

Змінна `IADE_DOXYGEN_OUT_HTML_NAME` сконфігурує назву для директорії HTML документації (передається до файлу `Doxyfile`).
