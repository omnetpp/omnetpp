//==========================================================================
//  IMAGECACHE.CC - part of
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

#include <cstring>
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QRegularExpression>
#include <QDebug>

#include "common/fileutil.h"
#include "common/stringtokenizer.h"
#include "imagecache.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

ImageCache::ImageCache()
{
    unknownImage = new QImage(":/objects/icons/objects/unknown.png");
}

ImageCache::~ImageCache()
{
    delete unknownImage;
}

void ImageCache::loadImages(const char *path)
{
    //TODO
    //global tcl_platform bitmaps bitmap_ctr

    //TODO
    //# On Windows, we use ";" to separate directories in $path. Using ":" (the
    //# Unix separator) would cause trouble with dirs containing drive letter
    //# (like "c:\bitmaps"). Using a space is also not an option (think of
    //# "C:\Program Files\...").
    StringTokenizer dir(path, PATH_SEPARATOR);
    while(dir.hasMoreTokens())
    {
        //TODO
        //puts -nonewline "Loading images from $dir: "
        doLoadImages(dir.nextToken());
        //puts ""
    }

    //if {$bitmap_ctr==0} {
    //  puts "*** no images (gif) in $path"
    //}
    //puts ""
}

void ImageCache::doLoadImages(const char *dir, const char *prefix)
{
    //TODO global bitmaps bitmap_ctr

    QDir content(dir);
    QStringList filters;
    filters << "*.gif" << "*.png";
    content.setNameFilters(filters);
    content.setFilter(QDir::Files);

    // # load bitmaps from this directory
    for (size_t i = 0; i < content.count(); ++i) {
        QString fileName = content.absoluteFilePath(content[i]);
        // QPixmap: Cannot create a QPixmap when no GUI is being used
        // QPixmap: Must construct a QApplication before a QPaintDevice
        QImage *image = new QImage(fileName);

        if (image->isNull())
            printf("Could not load image %s\n", fileName.toStdString().c_str());

        QStringList stringList = content[i].split(".");
        std::string key = prefix;
        for (int i = 0; i < stringList.size() - 1; ++i)
            key += stringList[i].toStdString();
        imagesWithSize[key] = image;
    }

    // # recurse into subdirs
    content.setNameFilters(QStringList("*"));
    content.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    for (size_t i = 0; i < content.count(); ++i) {
        QString subDir = content.absoluteFilePath(content[i]);
        QString subPrefix = prefix + content[i] + "/";
        doLoadImages(subDir.toStdString().c_str(), subPrefix.toStdString().c_str());
    }
}

const char *ImageCache::sizePostfix(IconSize size)
{
    switch (size) {
        case EXTRA_SMALL: return "_xs";
        case VERY_SMALL:  return "_vs";
        case SMALL:       return "_s";
        case NORMAL:      return "";
        case LARGE:       return "_l";
        case VERY_LARGE:  return "_vl";
        case EXTRA_LARGE: return "_xl";
        default: Q_ASSERT(false); return ""; // returning just to silence a warning...
    }
}

QImage *ImageCache::getImage(const char *name, const char *size)
{
    static QRegularExpression revs("v.*s.*", QRegularExpression::OptimizeOnFirstUsageOption);
    static QRegularExpression revl("v[^s]*l.*", QRegularExpression::OptimizeOnFirstUsageOption);

    IconSize imageSize = NORMAL;
    if (size && size[0]) {
        if (size[0] == 's')
            imageSize = SMALL;
        else if (size[0] == 'l')
            imageSize = LARGE;
        else if (revs.match(size).hasMatch())
            imageSize = VERY_SMALL;
        else if (revl.match(size).hasMatch())
            imageSize = VERY_LARGE;
    }

    return getImage(name, imageSize);
}

QImage *ImageCache::getImage(const char *name, IconSize size)
{
    QString nameWithSize = name;
    const char *postfix = sizePostfix(size);
    if (!nameWithSize.endsWith(postfix))
        nameWithSize += postfix;
    return getImage((nameWithSize).toStdString().c_str());
}

QImage *ImageCache::getImage(const char *nameWithSize)
{
    auto i = imagesWithSize.find(nameWithSize);
    return i != imagesWithSize.end() ? i->second : unknownImage;
}

}  // namespace qtenv
}  // namespace omnetpp

