//==========================================================================
//  IMAGECACHE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
class QIcon;
class QString;

namespace omnetpp {
namespace qtenv {

enum IconSize { EXTRA_SMALL, VERY_SMALL, SMALL, NORMAL, LARGE, VERY_LARGE, EXTRA_LARGE };

class QTENV_API ImageCache
{
private:
    std::unordered_map<std::string, QImage*> imagesWithSize;
    std::unordered_map<const char *, QIcon*> objectIcons; // the keys are returned by getObjectIconName

    void doLoadImage(const QString& fileName, const std::string& imageName);
    void doLoadImages(const char *dir, const char *prefix = "");

    // This should only be used for images that are loaded by this ImageCache,
    // since it uses the image pointer as part of the key.
    QPixmap makeTintedPixmapCached(QImage *image, const QColor &tintColor, double tintAmount);

    // The one with the red questionmarks, from the icons.qrc file.
    // This is returned when no image is found with the given name.
    QImage *unknownImage;

    static const char *sizePostfix(IconSize size);

    // A reference to the out member of EnvirBase (Qtenv),
    // using this to make output redirection work.
    std::ostream& out;
    bool verbose = true;

public:
    ImageCache(std::ostream& out);
    ~ImageCache();

    void setVerbose(bool verbose) {this->verbose = verbose;}
    void loadImages(const char *path);
    void loadImage(const char *fileName, const char *imageName = nullptr);

    QImage *getImage(const char *name, const char *size);
    QImage *getImage(const char *name, IconSize size);
    QImage *getImage(const char *nameWithSize);

    QPixmap makeTintedPixmap(QImage *image, const QColor& tintColor, double tintAmount);

    QPixmap getTintedPixmap(const char *name, const char *size, const QColor& tintColor, double tintAmount);
    QPixmap getTintedPixmap(const char *nameWithSize, const QColor& tintColor, double tintAmount);

    QIcon getObjectIcon(cObject *object);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_IMAGECACHE_H
