#-------------------------------------------------
#
# Project created by QtCreator 2015-03-13T00:38:46
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtenv
TEMPLATE = lib
CONFIG += static warn_off
DEFINES += "BUILDING_QTENV"
QMAKE_CXXFLAGS += $$(OPP_CFLAGS)
QMAKE_CFLAGS += $$(OPP_CFLAGS)

SOURCES += mainwindow.cc arrow.cc figurerenderers.cc histograminspector.cc logbuffer.cc outputvectorinspector.cc tkutil.cc canvasinspector.cc gateinspector.cc inspector.cc loginspector.cc qtenv.cc watchinspector.cc canvasrenderer.cc genericobjectinspector.cc inspectorfactory.cc moduleinspector.cc tkcmd.cc componenthistory.cc graphicspatharrowitem.cc layouterenv.cc osxproctype.cc tklib.cc

HEADERS += mainwindow.h arrow.h componenthistory.h graphicspatharrowitem.h layouterenv.h moduleinspector.h qtenv.h watchinspector.h canvasinspector.h figurerenderers.h histograminspector.h logbuffer.h outputvectorinspector.h tk-dummy.h canvasrenderer.h gateinspector.h inspectorfactory.h loginspector.h qtdefs.h tklib.h circularbuffer.h genericobjectinspector.h inspector.h mainwindow.h qtenvdefs.h tkutil.h


INCLUDEPATH += ../../../../src ../../../../include 
# next line is for the QtCreator (not needed for build)
INCLUDEPATH += .. ../../include

FORMS += mainwindow.ui
