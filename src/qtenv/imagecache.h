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

#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <string>
#include <map>

class QImage;
class QPixmap;
class QString;

enum IconSize { EXTRA_SMALL, VERY_SMALL, SMALL, NORMAL, LARGE, VERY_LARGE, EXTRA_LARGE };

class ImageCache
{
private:
    std::map<QString, QImage*> imagesWithSize;
    std::map<QString, std::map<IconSize, QImage*>> images;

    void doLoadImages(const char *dir, const char *prefix = "");

public:
    ImageCache();

    void loadImages(const char *path);
    QImage *getImage(const char *name, IconSize size);
    QImage *getImage(const char *nameWithSize);
};

#endif // IMAGECACHE_H
