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

TARGET = qtenv
TEMPLATE = lib

MAKEFILE_GENERATOR = UNIX
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

# the QMAKE_MOC variable is not available without this with Qt5, so our defines in OPP_CFLAGS didn't get applied
greaterThan(QT_MAJOR_VERSION, 4): load(moc)

# IMPORTANT: on turn off the option to generate both debug and release version
# we need only one of them not both and this is the default setting on Windows
# but sadly it generates broken makefiles
CONFIG *= static c++11 qt
CONFIG -= debug_and_release
CONFIG -= warn_on warn_off
DEFINES += "BUILDING_QTENV" "QT_NO_EMIT"
WARNING_FLAGS *= -Wall -Wextra -Wno-unused-parameter
!win32: WARNING_FLAGS *= -Wno-inconsistent-missing-override
QMAKE_CXXFLAGS += $$(OPP_CFLAGS) $$WARNING_FLAGS
QMAKE_CFLAGS += $$(OPP_CFLAGS) $$WARNING_FLAGS
# we have to add all defines to the MOC compiler command line to correctly parse the code
QMAKE_MOC += $$(OPP_DEFINES)

# add QT modules
QT *= core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets printsupport

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
    componenthistory.cc \
    graphicspatharrowitem.cc \
    layouterenv.cc \
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
    modulecanvasviewer.cc \
    filteredobjectlistdialog.cc \
    inspectorlistbox.cc \
    inspectorlistboxview.cc \
    comboselectiondialog.cc \
    compoundmoduleitem.cc \
    connectionitem.cc \
    messageitem.cc \
    canvasviewer.cc \
    layersdialog.cc \
    fileeditor.cc \
    cameramanipulators.cc

HEADERS += mainwindow.h \
    arrow.h \
    componenthistory.h \
    graphicspatharrowitem.h \
    layouterenv.h \
    moduleinspector.h \
    qtenv.h \
    watchinspector.h \
    canvasinspector.h \
    figurerenderers.h \
    histograminspector.h \
    logbuffer.h \
    outputvectorinspector.h \
    canvasrenderer.h \
    gateinspector.h \
    inspectorfactory.h \
    loginspector.h \
    circularbuffer.h \
    genericobjectinspector.h \
    inspector.h \
    qtenvdefs.h \
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
    modulecanvasviewer.h \
    filteredobjectlistdialog.h \
    inspectorlistbox.h \
    inspectorlistboxview.h \
    comboselectiondialog.h \
    compoundmoduleitem.h \
    connectionitem.h \
    messageitem.h \
    canvasviewer.h \
    layersdialog.h \
    fileeditor.h \
    cameramanipulators.h

# include path is relative to the current build directory (e.g. out/src/gcc-debug/qtenv)
INCLUDEPATH += ../../../../src ../../../../include 

# next line is for the QtCreator only to be able to show the OMNeT++ sources (not needed for the actual build process)
INCLUDEPATH += .. ../../include

FORMS += mainwindow.ui \
    runselectiondialog.ui \
    stopdialog.ui \
    logfinddialog.ui \
    logfilterdialog.ui \
    preferencesdialog.ui \
    rununtildialog.ui \
    filteredobjectlistdialog.ui \
    comboselectiondialog.ui \
    layersdialog.ui \
    fileeditor.ui

RESOURCES += \
    icons.qrc
