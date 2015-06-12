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

#include "imagecache.h"
#include "common/stringtokenizer.h"
#include <cstring>
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QRegExp>

#include <QDebug>

ImageCache::ImageCache()
{
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
    StringTokenizer dir(path, ":;");
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

    //# load bitmaps from this directory
    for(int i = 0; i < content.count(); ++i)
    {
        QString fileName = content.absoluteFilePath(content[i]);
        //QPixmap: Cannot create a QPixmap when no GUI is being used
        //QPixmap: Must construct a QApplication before a QPaintDevice
        QImage *image = new QImage(fileName);

        if(image->isNull())
            printf("Could not load image %s\n", fileName.toStdString().c_str());

        QStringList stringList = content[i].split(".");
        QString key = prefix;
        for(int i = 0; i < stringList.size() - 1; ++i)
            key += stringList[i];
        imagesWithSize[key] = image;

        stringList = key.split("_");
        key = "";
        for(int i = 0; i < stringList.size() - 1; ++i)
            key += stringList[i];

        QString size = stringList[stringList.size() - 1];
        if(size == "vl")
            images[key][VERY_LARGE] = image;
        else if (size == "xl")
            images[key][EXTRA_LARGE] = image;
        else if (size == "l")
            images[key][LARGE] = image;
        else if (size == "s")
            images[key][SMALL] = image;
        else if (size == "vs")
            images[key][VERY_SMALL] = image;
        else if (size == "xs")
            images[key][EXTRA_LARGE] = image;
        else
            images[key][NORMAL] = image;
    }

    //# recurse into subdirs
    content.setNameFilters(QStringList("*"));
    content.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i = 0; i < content.count(); ++i)
    {
        QString subDir = content.absoluteFilePath(content[i]);
        QString subPrefix = prefix + content[i] + "/";
        doLoadImages(subDir.toStdString().c_str(), subPrefix.toStdString().c_str());
    }
}

QImage *ImageCache::getImage(const char *name, IconSize size)
{
    auto it = images.find(name);
    if(it != images.end() && it->second.find(size) != it->second.end())
        return images[QString(name)][size];
    return nullptr;
}

QImage *ImageCache::getImage(const char *nameWithSize)
{
    if(imagesWithSize.find(nameWithSize) != imagesWithSize.end())
        return imagesWithSize[nameWithSize];
    return nullptr;
}
