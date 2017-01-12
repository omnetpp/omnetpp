//==========================================================================
//  IMAGECACHE.CC - part of
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

#include <cstring>
#include <iostream>
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

ImageCache::ImageCache(std::ostream& out): out(out)
{
    unknownImage = new QImage(":/objects/unknown");
}

ImageCache::~ImageCache()
{
    delete unknownImage;
    for (auto i : imagesWithSize)
        delete i.second;
}

void ImageCache::loadImages(const char *path)
{
    //# On Windows, we use ";" to separate directories in $path. Using ":" (the
    //# Unix separator) would cause trouble with dirs containing drive letter
    //# (like "c:\bitmaps"). Using a space is also not an option (think of
    //# "C:\Program Files\...").

    out << std::endl;
    StringTokenizer dir(path, PATH_SEPARATOR);

    while (dir.hasMoreTokens()) {
        auto cd = dir.nextToken();
        out << "Loading images from '" << cd << "':";
        doLoadImages("", toAbsolutePath(cd).c_str());
        out << std::endl;
    }

    if (imagesWithSize.empty())
        out << "*** no images in '" << path << "'";

    out << std::endl;
}

void ImageCache::loadImage(const char *fileName, const char *imageName)
{
    if (imageName)
        doLoadImage(fileName, imageName);
    else {
        std::string dir, name;
        splitFileName(fileName, dir, name);
        size_t pos = name.rfind('.');
        if (pos != std::string::npos)
            name = std::string(name, 0, pos);
        doLoadImage(fileName, name);
    }
}

void ImageCache::doLoadImage(const QString& fileName, const std::string& imageName)
{
    if (imagesWithSize.find(imageName) != imagesWithSize.end())
        return; // maybe print: not overwriting existing image. TODO: return true, skipping silently?

    QImage *image = new QImage(fileName);

    if (image->isNull()) {
        out << "Could not load image '" << fileName.toStdString() << "'" << std::endl;
        delete image;
    }
    else
        imagesWithSize[imageName] = image;
}

void ImageCache::doLoadImages(const char *dir, const char *prefix)
{
    QDir content(prefix + QString("/") + dir);
    QStringList filters;
    filters << "*.gif" << "*.png" << "*.jpg";
    content.setNameFilters(filters);
    content.setFilter(QDir::Files);

    out << " " << dir << "*: " << content.count() << " ";

    // load bitmaps from this directory
    for (size_t i = 0; i < content.count(); ++i) {
        QStringList stringList = content[i].split(".");
        std::string key = std::string(dir);
        for (int i = 0; i < stringList.size() - 1; ++i)
            key += stringList[i].toStdString();

        doLoadImage(content.absoluteFilePath(content[i]), key);
    }

    // recurse into subdirs
    content.setNameFilters(QStringList("*"));
    content.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    for (size_t i = 0; i < content.count(); ++i) {
        QString subDir = dir + content[i] + "/";
        doLoadImages(subDir.toStdString().c_str(), prefix);
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
    return getImage(nameWithSize.toUtf8());
}

QImage *ImageCache::getImage(const char *nameWithSize)
{
    auto i = imagesWithSize.find(nameWithSize);
    return i != imagesWithSize.end() ? i->second : unknownImage;
}

}  // namespace qtenv
}  // namespace omnetpp
