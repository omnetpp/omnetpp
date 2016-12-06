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
#include <unordered_map>
#include "qtenvdefs.h"

class QImage;
class QPixmap;
class QString;

namespace omnetpp {
namespace qtenv {

enum IconSize { EXTRA_SMALL, VERY_SMALL, SMALL, NORMAL, LARGE, VERY_LARGE, EXTRA_LARGE };

class QTENV_API ImageCache
{
private:
    std::unordered_map<std::string, QImage*> imagesWithSize;

    void doLoadImage(const QString& fileName, const std::string& imageName);
    void doLoadImages(const char *dir, const char *prefix = "");

    // The one with the red questionmarks, from the icons.qrc file.
    // This is returned when no image is found with the given name.
    QImage *unknownImage;

    static const char *sizePostfix(IconSize size);

    // A reference to the out member of EnvirBase (Qtenv),
    // using this to make output redirection work.
    std::ostream& out;

public:
    ImageCache(std::ostream& out);
    ~ImageCache();

    void loadImages(const char *path);
    void loadImage(const char *fileName, const char *imageName = nullptr);

    QImage *getImage(const char *name, const char *size);
    QImage *getImage(const char *name, IconSize size);
    QImage *getImage(const char *nameWithSize);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_IMAGECACHE_H
