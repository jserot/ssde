QT       += widgets core gui

QMAKE_PROJECT_NAME = ssde
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.6

LIBS += -L../qgv/lib -lQGVCore
INCLUDEPATH += ../qgv/QGVCore
DEPENDPATH += ../gqv/QGVCore

#GraphViz librairie
!include(../qgv/QGVCore/GraphViz.pri) {
     error("fail open GraphViz.pri")
 }

HEADERS += include/nlohmann_json.h \
           transition.h  \
           state.h  \
           fsd.h  \
           properties.h \
           mainwindow.h
SOURCES += transition.cpp \
           state.cpp \
           fsd.cpp \
           properties.cpp \
           main.cpp \
           mainwindow.cpp

RESOURCES += resources.qrc
