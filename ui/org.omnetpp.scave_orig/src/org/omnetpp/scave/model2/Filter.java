/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.ResultFileManager;


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

    public boolean isValid()
    {
        return isValid(filterPattern);
    }

    public static boolean isValid(String text)
    {
        try {
            ResultFileManager.checkPattern(text);
        } catch (Exception e) {
            return false; // apparently not valid
        }
        return true;
    }
}
