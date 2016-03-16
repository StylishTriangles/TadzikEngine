TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -static

SOURCES += main.cpp

HEADERS += \
    display.h \
    io.h \
    game.h

