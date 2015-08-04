//==========================================================================
//  IMAGECACHE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_IMAGECACHE_H
#define __OMNETPP_QTENV_IMAGECACHE_H

#include <string>
#include <map>
#include "qtenvdefs.h"

class QImage;
class QPixmap;
class QString;

namespace omnetpp {
namespace qtenv {

enum IconSize { EXTRA_SMALL, VERY_SMALL, SMALL, NORMAL, LARGE, VERY_LARGE, EXTRA_LARGE };

class ImageCache
{
private:
    std::map<QString, QImage*> imagesWithSize;

    void doLoadImages(const char *dir, const char *prefix = "");

    // The one with the red questionmarks, from the icons.qrc file.
    // This is returned when no image is found with the given name.
    QImage *unknownImage;

    static QString sizePostfix(IconSize size);

public:
    ImageCache();
    ~ImageCache();

    void loadImages(const char *path);
    QImage *getImage(const char *name, const char *size);
    QImage *getImage(const char *name, IconSize size);
    QImage *getImage(const char *nameWithSize);
};

} // namespace qtenv
} // namespace omnetpp

#endif // IMAGECACHE_H
