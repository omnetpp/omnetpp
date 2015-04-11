/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 *
 */
package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.Comparator;

import com.evelopers.unimod.core.stateworks.ClassElement;

final class ClassElementComparator implements Comparator {
    public int compare(Object o1, Object o2) {
        ClassElement ce1 = (ClassElement) o1;
        ClassElement ce2 = (ClassElement) o2;
        return ce1.getName().compareTo(ce2.getName());
    }
}