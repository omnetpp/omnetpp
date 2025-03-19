//==========================================================================
//  RANDOMICONGEN.H - part of
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

#ifndef __OMNETPP_QTENV_RANDOMICONGEN_H
#define __OMNETPP_QTENV_RANDOMICONGEN_H

#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QRandomGenerator>
#include <QtCore/QHash>
#include <QtCore/QString>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

/**
 * A class that generates random icons similar to GitHub avatars.
 * The icons are deterministic based on a seed string.
 */
class QTENV_API RandomIconGenerator
{
private:
    // Cache generated icons to avoid regenerating the same icon
    static QHash<QString, QIcon> iconCache;

    // Generate a random color based on the seed
    static QColor generateColor(QRandomGenerator &rng, bool light);

public:
    /**
     * Generates a random icon based on a seed string.
     * The same seed will always produce the same icon.
     */
    static QIcon generateIcon(const QString &seed, int size = 32);

    /**
     * Clears the icon cache
     */
    static void clearCache();
};


} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_RANDOMICONGEN_H
