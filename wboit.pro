# qt example of OIT

QT += core gui opengl

TARGET = wboit
TEMPLATE = app

SOURCES += \
    mesh.cpp \
    glwidget.cpp \
    main.cpp

HEADERS += \
    mesh.h \
    glwidget.h

RESOURCES += \
    shaders.qrc

