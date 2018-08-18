cache()

CONFIG += qt \
    warn_on \
    debug_and_release \
    c++11 \
    $$[build_mode]
QT = core \
    widgets \
    network \
    script
static_build {
    win32 {
        QT += axcontainer
    }
    else {
        message(Error: Webkit is not compatible with STATIC builds!!)
    }
}
else {
    QT += webenginewidgets
}

CONFIG += plugins_debug_off
TEMPLATE = app
DESTDIR = bin
UI_DIR = build/ui
MOC_DIR = build/moc
RCC_DIR = build/rcc
FORMS = ui/addvideo.ui \
    ui/searchvideositem.ui \
    ui/bugreport.ui \
    ui/checkupdates.ui \
    ui/completedpopup.ui \
    ui/credits.ui \
    ui/customalphablend.ui \
    ui/downloadlog.ui \
    ui/dragdrop.ui \
    ui/information.ui \
    ui/infoview.ui \
    ui/loading.ui \
    ui/mainform.ui \
    ui/newlanguages.ui \
    ui/options.ui \
    ui/scheduleitemedit.ui \
    ui/searchvideos.ui \
    ui/updatecenter.ui \
    ui/upspassword.ui \
    ui/welcomedonate.ui \
    ui/winvistaupdatesmsg.ui \
    ui/searchvideossearchblock.ui \
    ui/searchvideoscustomize.ui \
    ui/customdownloadtitle.ui \
    ui/changelog.ui \
    ui/searchvideosnoresults.ui \
    ui/winvistadownloadsmsg.ui \
    ui/whatsnew.ui \
    ui/multiurls.ui \
    ui/adultspermission.ui
HEADERS = src/forms/addvideoimpl.h \
    src/forms/searchvideositemimpl.h \
    src/forms/bugreportimpl.h \
    src/forms/checkupdatesimpl.h \
    src/forms/completedpopupimpl.h \
    src/forms/creditsimpl.h \
    src/forms/customalphablendimpl.h \
    src/forms/downloadlogimpl.h \
    src/forms/dragdropimpl.h \
    src/forms/informationimpl.h \
    src/forms/infoviewimpl.h \
    src/forms/loadingimpl.h \
    src/forms/mainformimpl.h \
    src/forms/newlanguagesimpl.h \
    src/forms/optionsimpl.h \
    src/forms/scheduleitemeditimpl.h \
    src/forms/searchvideosimpl.h \
    src/forms/updatecenterimpl.h \
    src/forms/upspasswordimpl.h \
    src/forms/welcomedonate.h \
    src/forms/windowsvistadetectedimpl.h \
    src/forms/searchvideossearchblockimpl.h \
    src/forms/searchvideoscustomizeimpl.h \
    src/forms/customdownloadtitleimpl.h \
    src/forms/changelogimpl.h \
    src/forms/searchvideosnoresultsimpl.h \
    src/forms/winvistadownloadsmsgimpl.h \
    src/forms/whatsnewimpl.h \
    src/forms/multiurlsimpl.h \
    src/forms/adultspermissionimpl.h \
    src/pakunpak/pakunpak.h \
    src/searchvideosscriptclass.h \
    src/checkupdatesworker.h \
    src/http.h \
    src/httpscriptclass.h \
    src/languages.h \
    src/newlanguages.h \
    src/options.h \
    src/programversion.h \
    src/progressbardelegate.h \
    src/schedule.h \
    src/searchvideos.h \
    src/sessionmanager.h \
    src/tools.h \
    src/toolsscriptclass.h \
    src/trackerreport.h \
    src/updates.h \
    src/videoconvert.h \
    src/videoconverttypes.h \
    src/videodownload.h \
    src/videoinformation.h \
    src/videoitem.h \
    src/videolistcontroller.h \
    src/xprogressbar.h \
    src/rtmp.h \
    src/youtubedl.h
SOURCES = src/forms/addvideoimpl.cpp \
    src/forms/searchvideositemimpl.cpp \
    src/forms/bugreportimpl.cpp \
    src/forms/checkupdatesimpl.cpp \
    src/forms/completedpopupimpl.cpp \
    src/forms/creditsimpl.cpp \
    src/forms/customalphablendimpl.cpp \
    src/forms/downloadlogimpl.cpp \
    src/forms/dragdropimpl.cpp \
    src/forms/informationimpl.cpp \
    src/forms/infoviewimpl.cpp \
    src/forms/loadingimpl.cpp \
    src/forms/mainformimpl.cpp \
    src/forms/newlanguagesimpl.cpp \
    src/forms/optionsimpl.cpp \
    src/forms/scheduleitemeditimpl.cpp \
    src/forms/searchvideosimpl.cpp \
    src/forms/updatecenterimpl.cpp \
    src/forms/upspasswordimpl.cpp \
    src/forms/welcomedonate.cpp \
    src/forms/windowsvistadetectedimpl.cpp \
    src/forms/searchvideossearchblockimpl.cpp \
    src/forms/searchvideoscustomizeimpl.cpp \
    src/forms/customdownloadtitleimpl.cpp \
    src/forms/changelogimpl.cpp \
    src/forms/searchvideosnoresultsimpl.cpp \
    src/forms/winvistadownloadsmsgimpl.cpp \
    src/forms/whatsnewimpl.cpp \
    src/forms/multiurlsimpl.cpp \
    src/forms/adultspermissionimpl.cpp \
    src/searchvideosscriptclass.cpp \
    src/checkupdatesworker.cpp \
    src/http.cpp \
    src/httpscriptclass.cpp \
    src/languages.cpp \
    src/main.cpp \
    src/newlanguages.cpp \
    src/options.cpp \
    src/pakunpak/pakunpak.cpp \
    src/progressbardelegate.cpp \
    src/schedule.cpp \
    src/searchvideos.cpp \
    src/sessionmanager.cpp \
    src/tools.cpp \
    src/toolsscriptclass.cpp \
    src/trackerreport.cpp \
    src/updates.cpp \
    src/videoconvert.cpp \
    src/videodownload.cpp \
    src/videoinformation.cpp \
    src/videoitem.cpp \
    src/videolistcontroller.cpp \
    src/xprogressbar.cpp \
    src/rtmp.cpp \
    src/youtubedl.cpp
TRANSLATIONS = resources/translations/xVST_br.ts \
    resources/translations/xVST_ca.ts \
    resources/translations/xVST_cz.ts \
    resources/translations/xVST_da.ts \
    resources/translations/xVST_de.ts \
    resources/translations/xVST_en.ts \
    resources/translations/xVST_es.ts \
    resources/translations/xVST_fr.ts \
    resources/translations/xVST_gl.ts \
    resources/translations/xVST_hu.ts \
    resources/translations/xVST_it.ts \
    resources/translations/xVST_kr.ts \
    resources/translations/xVST_pl.ts \
    resources/translations/xVST_ro.ts \
    resources/translations/xVST_sv.ts \
    resources/translations/xVST_du.ts \
    resources/translations/xVST_jp.ts \
    resources/translations/xVST_ru.ts \
    resources/translations/xVST_tr.ts \
    resources/translations/xVST_fi.ts \
    resources/translations/xVST_zh_TW.ts \
    resources/translations/template_for_new_translations.ts
RESOURCES = resources/resources.qrc
unix {
    OBJECTS_DIR = build/o/unix
    TARGET = xvst
}
macx {
    QT -= webkitwidgets
    QT += macextras
    ICON += $${PWD}/resources/icons/MacOSX.icns
    OBJECTS_DIR = build/o/mac
    TARGET = xVideoServiceThief
    QMAKE_INFO_PLIST = $${PWD}/Info.plist
        plist.target = "$${OUT_PWD}/$${DESTDIR}/$${TARGET}.app/Contents/Info.plist"
        plist.depends = $${PWD}/Info.plist
        plist.commands = $(DEL_FILE) \"$${OUT_PWD}/$${DESTDIR}/$${TARGET}.app/Contents/Info.plist\" $$escape_expand(\n\t) \
                         $(COPY_FILE) $${PWD}/Info.plist \"$${OUT_PWD}/$${DESTDIR}/$${TARGET}.app/Contents/Info.plist\"
        QMAKE_EXTRA_TARGETS = plist
        PRE_TARGETDEPS += $$plist.target
    LIBS += -framework Cocoa -framework WebKit
    OBJECTIVE_SOURCES += \
        src/mac_only/mac_tools.mm \
        src/mac_only/WebKitClass.mm \
        src/mac_only/qmactoolbarext.mm \
        src/mac_only/NSButtonToolbar.mm
    HEADERS += \
        src/mac_only/mac_tools.h \
        src/mac_only/WebKitClass.h \
        src/mac_only/qmactoolbarext.h \
        src/mac_only/NSButtonToolbar.h
    CONFIG(release, debug|release) {
        message(Release build! Archs: 32 and 64 bits)
        CONFIG += x86 x86_64
    }
    CONFIG(debug, debug|release):message(Debug build! Archs: Current architecture only)
}
win32 {
    RC_FILE += resources/xVST.rc
    OBJECTS_DIR = build/o/win32
}
frameworks_build {
    DEFINES += FRAMEWORKS_BUILD
    message(Building with Framewroks)
}
static_build {
    DEFINES += STATIC_BUILD
    QTPLUGIN += qico
    message(Building Static version)
}
plugins_debug_on {
    DEFINES += xVST_DEBUG_PLUGINS_ON
    QT += scripttools
    message(Plugins debugger is ON)
}
else {
    DEFINES += xVST_DEBUG_PLUGINS_OFF
    message(Plugins debugger is OFF)
}
include(src/qtsingleapplication/src/qtsingleapplication.pri)
