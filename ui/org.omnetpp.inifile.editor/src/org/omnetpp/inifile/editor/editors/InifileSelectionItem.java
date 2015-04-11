/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.editors;

import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

/**
 * An item in structured selections (IStructuredSelection) published
 * by InifileEditor. With text-based editing, the selected section/key
 * is where the cursor is currently located; with form-based editing,
 * it may be the section/key edited by the currently focused form control.
 *
 * @author Andras
 */
public class InifileSelectionItem {
    private IInifileDocument document;
    private InifileAnalyzer analyzer;
    private String section;
    private String key;

    public InifileSelectionItem(IInifileDocument document, InifileAnalyzer analyzer, String section, String key) {
        this.document = document;
        this.analyzer = analyzer;
        this.section = section;
        this.key = key;
    }

    /**
     * The inifile document which contains the selection.
     */
    public IInifileDocument getDocument() {
        return document;
    }

    /**
     * The inifile analyzer.
     */
    public InifileAnalyzer getAnalyzer() {
        return analyzer;
    }

    /**
     * The inifile section selected, or null.
     */
    public String getSection() {
        return section;
    }

    /**
     * The inifile key selected (within the section), or null.
     */
    public String getKey() {
        return key;
    }

    @Override
    public String toString() {
        return "doc="+document+" section="+section+" key="+key;
    }

    /* generated */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        final InifileSelectionItem other = (InifileSelectionItem) obj;
        if (document == other.document) {
            if (other.document != null)
                return false;
        }
        else if (!document.equals(other.document))
            return false;
        if (key == null) {
            if (other.key != null)
                return false;
        }
        else if (!key.equals(other.key))
            return false;
        if (section == null) {
            if (other.section != null)
                return false;
        }
        else if (!section.equals(other.section))
            return false;
        return true;
    }
}
