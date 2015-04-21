#-------------------------------------------------
#
# Project created by QtCreator 2015-03-13T00:38:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtenv-ui
TEMPLATE = lib
CONFIG += static

SOURCES += mainwindow.cpp \
    qtenv.cc \
    graphicspatharrowitem.cc \
    figurerenderers.cc \
    canvasrenderer.cc

HEADERS  += mainwindow.h \
    qtenv.h \
    qtdefs.h \
    canvasrenderer.h \
    figurerenderers.h \
    graphicspatharrowitem.h

FORMS    += mainwindow.ui
