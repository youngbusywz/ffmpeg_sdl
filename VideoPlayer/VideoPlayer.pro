#-------------------------------------------------
#
# Project created by QtCreator 2020-08-10T16:51:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoPlayer
TEMPLATE = app
DESTDIR     = $$PWD/bin

SOURCES += main.cpp\
        Widget.cpp \
    VideoDecode.cpp \
    SDLRender.cpp

HEADERS  += Widget.h \
    VideoDecode.h \
    videoplayerheader.h \
    SDLRender.h

FORMS    += Widget.ui


win32: LIBS += -L$$PWD/sdl/lib/x86/ -lSDL2main
win32: LIBS += -L$$PWD/sdl/lib/x86/ -lSDL2

INCLUDEPATH += $$PWD/sdl/include
DEPENDPATH += $$PWD/sdl/lib/x86

win32:LIBS += -L$$PWD/ffmpeg/lib/ -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale

INCLUDEPATH += $$PWD/ffmpeg/include
DEPENDPATH += $$PWD/ffmpeg/lib
