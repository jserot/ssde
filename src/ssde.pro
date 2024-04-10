!include(../config) { error("Cannot open config file. Run configure script in top directory") }

QT       += core widgets gui

QMAKE_PROJECT_NAME = ssde
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.6

TARGET = ssde
TEMPLATE = app

INCLUDEPATH += $$QGVDIR/$$PLATFORM_TYPE/lib
LIBS += -L$$QGVDIR/$$PLATFORM_TYPE/lib -lQGVCore
DEPENDPATH += $$QGVDIR/$$PLATFORM_TYPE/lib

!include(./GraphViz.pri) { error("Cannot open GraphViz.pri file") }

HEADERS += include/nlohmann_json.h \
           qt_compat.h \
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
