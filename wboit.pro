# qt example of OIT

CONFIG += c++17
QT += core gui opengl widgets openglwidgets
TARGET = wboit
TEMPLATE = app


INCLUDEPATH += "C:/Users/jaime/Desktop/glm/"

SOURCES += \
    mesh.cpp \
    glwidget.cpp \
    main.cpp

HEADERS += \
    mesh.h \
    glwidget.h

RESOURCES += \
    shaders.qrc

