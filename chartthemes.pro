QT += charts
requires(qtConfig(combobox))

HEADERS += \
    themewidget.h

SOURCES += \
    main.cpp \
    themewidget.cpp

target.path = ./charts/chartthemes
INSTALLS += target

FORMS += \
    themewidget.ui
