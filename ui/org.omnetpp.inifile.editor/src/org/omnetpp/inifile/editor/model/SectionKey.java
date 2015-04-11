/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

/**
 * Value object class for storing a section name and key name together.
 */
public class SectionKey {
    public String section;
    public String key;

    public SectionKey(String section, String key) {
        this.section = section;
        this.key = key;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        return key.hashCode() + prime * section.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        return getClass()==obj.getClass() && section.equals(((SectionKey)obj).section) && key.equals(((SectionKey)obj).key);
    }
}