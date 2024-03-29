# This is just a "project file" for Qt Creator, not used for building.
# See the notes in ../qtenv.pro for more info.

TARGET = qtenv-osg

# so Qt Creator finds the Qt module headers and can parse the files properly
QT *= core gui opengl widgets printsupport

DEFINES += "QT_NO_KEYWORDS"

SOURCES += osgviewer.cc \
    cameramanipulators.cc

HEADERS += osgviewer.h \
    cameramanipulators.h

# include path is relative to the current build directory (e.g. out/src/gcc-debug/qtenv/osg)
INCLUDEPATH += ../../../../../src ../../../../../include

# next line is for the QtCreator only to be able to show the OMNeT++ sources (not needed for the actual build process)
INCLUDEPATH += .. ../.. ../../../include ../../../src
