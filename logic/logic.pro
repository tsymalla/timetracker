QT += sql
QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chartdataprovider.cpp \
    csvwriter.cpp \
    database.cpp \
    dataprovider.cpp \
    entrymodel.cpp \
    entryproxymodel.cpp \
    projectmodel.cpp \
    projecttreemodel.cpp \
    taskmodel.cpp

HEADERS += \
    chartdataprovider.h \
    csvwriter.h \
    database.h \
    databaseconfiguration.h \
    dataprovider.h \
    entrymodel.h \
    entryproxymodel.h \
    projectmodel.h \
    projecttreemodel.h \
    taskmodel.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
