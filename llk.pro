TEMPLATE = app
CONFIG += release warn-on

unix:!macx {
	LIBS = -lSDL
	INCLUDEPATH = /usr/include/SDL
}

win32 {
	debug {
		CONFIG += console
	}
}

# Input
HEADERS += board.h \
	   changebackgroundui.h \
           changebackgroundui.ui.h \
           highscore.h \
           highscorebase.ui.h \
           llk.h \
           llkrule.h \
           mainApp.ui.h \
           myfiledialog.h \
           qcolorbutton.h \
           qconfig.h \
           qurlrequester.h \
           tileset.h \
           general.h \
		   aboutgui.ui.h \
		   dialogwithcheckbox.h \
		   sound.h

SOURCES += board.cpp \
		   changebackgroundui.cpp \
           highscore.cpp \
           llk.cpp \
           llkrule.cpp \
           main.cpp \
           myfiledialog.cpp \
           qcolorbutton.cpp \
           qconfig.cpp \
           qurlrequester.cpp \
           tileset.cpp \
		   dialogwithcheckbox.cpp

unix:!macx {
	SOURCES += sound_sdl.cpp
}

win32 {
	SOURCES += sound_qt.cpp
}

macx {
	SOURCES += sound_qt.cpp
}

INTERFACES += highscorebase.ui aboutgui.ui mainApp.ui
TRANSLATIONS = llk_zh_CN.ts

TARGET = llk

unix {
	INSTALLDIR = $$(DESTDIR)/usr/share/apps/llk
	ICONSDIR = $$(DESTDIR)/usr/share/icons/hicolor

	target.path = $$(DESTDIR)/usr/bin

	desktop.path = $$(DESTDIR)/usr/share/applications/kde
	desktop.files = llk.desktop

	icons01.path = $$ICONSDIR/16x16/apps/
	icons01.files = icons/16x16/llk.png

	icons02.path = $$ICONSDIR/32x32/apps/
	icons02.files = icons/32x32/llk.png

	icons03.path = $$ICONSDIR/48x48/apps/
	icons03.files = icons/48x48/llk.png

	trans.path = $$INSTALLDIR
	trans.files = llk_zh_CN.qm

	pictures.path = $$INSTALLDIR
	pictures.files = pics/*.png pics/rfmin_logo.xpm

	icons16.path = $$INSTALLDIR/icons/16x16
	icons16.files = icons/16x16/*.png

	icons22.path = $$INSTALLDIR/icons/22x22
	icons22.files = icons/22x22/*.png

	sound.path = $$INSTALLDIR/wav
	sound.files = wav/*.wav

	INSTALLS += target trans desktop icons01 icons02 icons03 pictures \
		icons16 icons22 sound
}
