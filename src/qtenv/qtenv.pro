#-------------------------------------------------
#
# Project created by QtCreator 2015-03-13T00:38:46
#
#-------------------------------------------------
#
# To properly set up the project/build process for QtCreator you need to invoke only
# the 'make' command in this directory (by default QtCreator invokes also qmake)
#
# - be sure to start QtCreator from the command line (where the path contains the omnetpp/bin directory)
# - open this file as a project
# - select the "Projects" pane on the left
# - press "Configure Project" button
# - select the "Projects" pane again
# - turn off "Shadow build"
# - on the build steps, delete the "qmake" build step
# - optional (if you want to create release builds from QtCreator):
#   for the release configuration add the MODE=release argument to the make line
#

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

WARNING_FLAGS += "-Wall -Wextra -Wno-unused-parameter"

TARGET = qtenv
TEMPLATE = lib
CONFIG += static c++11
CONFIG -= warn_on warn_off
DEFINES += "BUILDING_QTENV"
QMAKE_CXXFLAGS += $$(OPP_CFLAGS) $$WARNING_FLAGS
QMAKE_CFLAGS += $$(OPP_CFLAGS) $$WARNING_FLAGS

SOURCES += mainwindow.cc \
    arrow.cc \
    figurerenderers.cc \
    histograminspector.cc \
    logbuffer.cc \
    outputvectorinspector.cc \
    canvasinspector.cc \
    gateinspector.cc \
    inspector.cc \
    loginspector.cc \
    qtenv.cc \
    watchinspector.cc \
    canvasrenderer.cc \
    genericobjectinspector.cc \
    inspectorfactory.cc \
    moduleinspector.cc \
    tkcmd.cc \
    componenthistory.cc \
    graphicspatharrowitem.cc \
    layouterenv.cc \
    osxproctype.cc \
    tklib.cc \
    stopdialog.cc \
    runselectiondialog.cc \
    imagecache.cc \
    treeitemmodel.cc \
    genericobjecttreemodel.cc \
    qtutil.cc \
    inspectorutil.cc \
    textviewerwidget.cc \
    textviewerproviders.cc \
    logfinddialog.cc \
    logfilterdialog.cc \
    timelinegraphicsview.cc \
    timelineinspector.cc \
    animator.cc \
    preferencesdialog.cc \
    objecttreeinspector.cc \
    osgcanvasinspector.cc \
    osgviewer.cc \
    rununtildialog.cc \
    submoduleitem.cc \
    modulecanvasviewer.cc

HEADERS += mainwindow.h arrow.h componenthistory.h graphicspatharrowitem.h layouterenv.h moduleinspector.h qtenv.h watchinspector.h canvasinspector.h figurerenderers.h histograminspector.h logbuffer.h outputvectorinspector.h tk-dummy.h canvasrenderer.h gateinspector.h inspectorfactory.h loginspector.h tklib.h circularbuffer.h genericobjectinspector.h inspector.h qtenvdefs.h \
    runselectiondialog.h \
    treeitemmodel.h \
    stopdialog.h \
    imagecache.h \
    genericobjecttreemodel.h \
    qtutil.h \
    inspectorutil.h \
    textviewerwidget.h \
    textviewerproviders.h \
    logfinddialog.h \
    logfilterdialog.h \
    timelinegraphicsview.h \
    timelineinspector.h \
    animator.h \
    preferencesdialog.h \
    objecttreeinspector.h \
    osgcanvasinspector.h \
    osgviewer.h \
    rununtildialog.h \
    submoduleitem.h \
    modulecanvasviewer.h

# include path is relative to the current build directory (e.g. out/src/gcc-debug/qtenv)
INCLUDEPATH += ../../../../src ../../../../include 

# next line is for the QtCreator only to be able to show the OMNeT++ sources (not needed for the actual build process)
INCLUDEPATH += .. ../../include

INCLUDEPATH += /usr/include/qt4/QtOpenGL /usr/X11R6/include  #FIXME HACK! needed of osg, but probably should go into configure

FORMS += mainwindow.ui \
    runselectiondialog.ui \
    canvasinspectorform.ui \
    stopdialog.ui \
    logfinddialog.ui \
    logfilterdialog.ui \
    preferencesdialog.ui \
    rununtildialog.ui

RESOURCES += \
    icons.qrc
