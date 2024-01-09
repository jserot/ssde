QT       += widgets core gui

QMAKE_PROJECT_NAME = ssde
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.6

LIBS += -L../../qgv/lib -lQGVCore
INCLUDEPATH += ../../qgv/QGVCore
DEPENDPATH += ../../gqv/QGVCore

#GraphViz librairie
!include(../../qgv/QGVCore/GraphViz.pri) {
     error("fail open GraphViz.pri")
 }

HEADERS += include/nlohmann_json.h \
           transition.h  \
           state.h  \
           model.h  \
           properties.h \
           mainwindow.h
SOURCES += transition.cpp \
           state.cpp \
           model.cpp \
           properties.cpp \
           mainwindow.cpp \
           main.cpp

RESOURCES += resources.qrc
