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
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

#include "omnetpp/cexception.h"
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "common/stringtokenizer.h"
#include "imagecache.h"
#include "qtutil.h"


using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

ImageCache::ImageCache(std::ostream& out): out(out)
{
    unknownImage = new QImage(":/objects/unknown");
    QPixmapCache::setCacheLimit(102400);
}

ImageCache::~ImageCache()
{
    delete unknownImage;
    for (auto i : imagesWithSize)
        delete i.second;
    for (auto i : objectIcons)
        delete i.second;
}

void ImageCache::loadImages(const char *path)
{
    //# On Windows, we use ";" to separate directories in $path. Using ":" (the
    //# Unix separator) would cause trouble with dirs containing drive letter
    //# (like "c:\bitmaps"). Using a space is also not an option (think of
    //# "C:\Program Files\...").

    if (verbose)
        out << std::endl;

    StringTokenizer dir(path, PATH_SEPARATOR);
    while (dir.hasMoreTokens()) {
        auto cd = dir.nextToken();
        if (verbose)
            out << "Loading images from '" << cd << "':";
        doLoadImages("", toAbsolutePath(cd).c_str());
        if (verbose)
            out << std::endl;
    }

    if (imagesWithSize.empty() && verbose)
        out << "*** no images in '" << path << "'";

    if (verbose)
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
        out << "Warning: Could not load image '" << fileName.toStdString() << "'" << std::endl;
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

    if (verbose)
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

QPixmap ImageCache::makeTintedPixmapCached(QImage *image, const QColor &tintColor, double tintAmount)
{
    QString key;
    QTextStream stream(&key);
    stream.setRealNumberNotation(QTextStream::FixedNotation);
    stream.setRealNumberPrecision(2);
    stream << image << "|" << tintColor.name() << "|" << tintAmount;

    QPixmap pixmap;

    if (QPixmapCache::find(key, &pixmap))
        return pixmap;

    pixmap = makeTintedPixmap(image, tintColor, tintAmount);

    QPixmapCache::insert(key, pixmap);

    return pixmap;
}

QPixmap ImageCache::makeTintedPixmap(QImage *image, const QColor &tintColor, double tintAmount)
{
    ASSERT(image);

    if (tintAmount == 0.0)
        return QPixmap::fromImage(*image);

    // This copies the data itself if the format is not already as
    // desired, but does not if it is. However, scanLine() will call
    // detach, so we can be sure we won't mess up any other QImage.
    // The conversion is necessary because with QImages created
    // from the data of a cFigure::Pixmap, the red and blue channels
    // got flipped by the colorization.
    // (Format_ARGB32 is endianness-dependent, so it is really B-G-R-A
    // in memory mapped to the R-G-B-A data of the Pixmap. I guess.)
    QImage tintedImage = image->convertToFormat(QImage::Format_ARGB32);

    double rdest = tintColor.redF();
    double gdest = tintColor.greenF();
    double bdest = tintColor.blueF();

    for (int y = 0; y < tintedImage.height(); y++) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(tintedImage.scanLine(y));
        for (int x = 0; x < tintedImage.width(); x++) {
            QRgb& pixel = scanLine[x];

            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);

            // transform - code taken from tkenv/tkcmd.cc (colorizeImage_cmd)
            int lum = (int)(0.2126*r + 0.7152*g + 0.0722*b);
            r = (int)((1-tintAmount)*r + tintAmount*lum*rdest);
            g = (int)((1-tintAmount)*g + tintAmount*lum*gdest);
            b = (int)((1-tintAmount)*b + tintAmount*lum*bdest);

            pixel = qRgba(r, g, b, qAlpha(pixel));
        }
    }

    return QPixmap::fromImage(tintedImage);
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

QPixmap ImageCache::getTintedPixmap(const char *name, const char *size, const QColor &tintColor, double tintAmount)
{
    return makeTintedPixmapCached(getImage(name, size), tintColor, tintAmount);
}

QPixmap ImageCache::getTintedPixmap(const char *nameWithSize, const QColor &tintColor, double tintAmount)
{
    return makeTintedPixmapCached(getImage(nameWithSize), tintColor, tintAmount);
}

QIcon ImageCache::getObjectIcon(cObject *object)
{
    const char *name = getObjectIconName(object);

    if (!containsKey(objectIcons, name))
        objectIcons[name] = new QIcon(QString(":/objects/") + name);

    return *objectIcons[name];
}


}  // namespace qtenv
}  // namespace omnetpp
