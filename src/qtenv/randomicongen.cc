//==========================================================================
//  RANDOMICONGEN.CC - part of
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

#include "randomicongen.h"

namespace omnetpp {
namespace qtenv {

QHash<QString, QIcon> RandomIconGenerator::iconCache;

QColor RandomIconGenerator::generateColor(QRandomGenerator &rng, bool light)
{
    int h = rng.bounded(360);
    int s = rng.bounded(120, 256);
    int l = light ? rng.bounded(190, 230) : rng.bounded(100, 130);
    return QColor::fromHsl(h, s, l);
}

QIcon RandomIconGenerator::generateIcon(const QString &seed, int size)
{
    // Check if we already have this icon in the cache
    QString cacheKey = seed + QString::number(size);
    if (iconCache.contains(cacheKey))
    return iconCache[cacheKey];

    // Create a deterministic random generator based on the seed
    QRandomGenerator rng(qHash(seed));

    // Generate a fg/bg colors with good contrast
    QColor bgColor = generateColor(rng, true);
    QColor fgColor = generateColor(rng, false);

    // Create a pixmap to draw on
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(0, 0, size, size, bgColor);

    // Decide on a grid size (3x3, 4x4, or 5x5)
    int gridSize = rng.bounded(3, 6);

    painter.setPen(Qt::NoPen);
    painter.setBrush(fgColor);

    // Generate a symmetric pattern
    for (int x = 0; x < (gridSize + 1) / 2; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            // Randomly decide whether to draw a cell
            if (rng.bounded(2) == 1) {
                // Draw the cell - compute coordinates directly to avoid rounding error accumulation
                int x1 = (size * x) / gridSize;
                int y1 = (size * y) / gridSize;
                int x2 = (size * (x + 1)) / gridSize;
                int y2 = (size * (y + 1)) / gridSize;
                painter.drawRect(x1, y1, x2 - x1, y2 - y1);

                // Draw the symmetric cell (except for the middle column in odd-sized grids)
                if (x != gridSize - 1 - x) {
                    int symX = gridSize - 1 - x;
                    int symX1 = (size * symX) / gridSize;
                    int symX2 = (size * (symX + 1)) / gridSize;
                    painter.drawRect(symX1, y1, symX2 - symX1, y2 - y1);
                }
            }
        }
    }

    painter.end();

    // Create an icon from the pixmap
    QIcon icon(pixmap);

    // Cache the icon
    iconCache[cacheKey] = icon;

    return icon;
}

void RandomIconGenerator::clearCache()
{
    iconCache.clear();
}

} // namespace qtenv
} // namespace omnetpp
