/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;


/**
 * Filter parameters for datasets. This basically wraps filterPattern.
 */
public class Filter {
    private String filterPattern;

    public Filter() {
    }

    public Filter(String filterText) {
        this.filterPattern = filterText==null ? "" : filterText;
    }

    public String getFilterPattern() {
        return filterPattern;
    }

    public void setFilterPattern(String text) {
        filterPattern = text;
    }
}
