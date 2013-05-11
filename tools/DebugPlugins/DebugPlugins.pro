CONFIG += qt \
    warn_on \
    debug_and_release \
    $$[build_mode]
QT = core \
	widgets \
	network \
	script \
	scripttools
DEFINES += xVST_DEBUG_PLUGINS_ON
TARGET = DebugPlugins
TEMPLATE = app
DESTDIR = bin
UI_DIR = build/ui
MOC_DIR = build/moc
RCC_DIR = build/rcc
OBJECTS_DIR += build/o/
SOURCES += src/main.cpp \
    src/forms/mainwindow.cpp \
    src/programsettings.cpp
HEADERS += src/forms/mainwindow.h \
    src/programsettings.h
FORMS += ui/mainwindow.ui
macx {
	QMAKE_INFO_PLIST = resources/Info.plist
	ICON += resources/icons/MacOSX.icns
	CONFIG(release, debug|release) {
		message(Release build! Archs: 32 and 64 bits)
		CONFIG += x86 x86_64
	}
	CONFIG(debug, debug|release):message(Debug build! Archs: Current architecture only)
}
win32 {
	RC_FILE += resources/DebugPlugins.rc
}
frameworks_build { 
    DEFINES += FRAMEWORKS_BUILD
    message(frameworks_build)
}
static_build { 
    DEFINES += STATIC_BUILD
    QTPLUGIN += qico qgif qjpeg
    message(static_build)
}
# include the common xVST classes
include(../Common/xvst-classes.pri)
