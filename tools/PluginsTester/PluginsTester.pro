QT += network \
    script \
    scripttools
TARGET = PluginsTester
TEMPLATE = app
DESTDIR = bin
UI_DIR = build/ui
MOC_DIR = build/moc
RCC_DIR = build/rcc
OBJECTS_DIR += build/o/
SOURCES += src/main.cpp \
    src/plugintester.cpp \
    src/forms/mainwindow.cpp \
    src/forms/working.cpp \
    src/programsettings.cpp
HEADERS += src/plugintester.h \
    src/forms/mainwindow.h \
    src/forms/working.h \
    src/programsettings.h
FORMS += ui/mainwindow.ui \
    ui/working.ui
macx {
	QMAKE_INFO_PLIST = Info.plist
}
# include the common xVST classes
include(../Common/xvst-classes.pri)
