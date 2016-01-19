#-------------------------------------------------
#
# Project created by QtCreator 2016-01-19T21:06:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qfdzipstream
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -lz


SOURCES += main.cpp \
    qfdzipstream.cpp \
    fdzipstream/fdzipstream.c

HEADERS += \
    qfdzipstream.h \
    fdzipstream/fdzipstream.h
