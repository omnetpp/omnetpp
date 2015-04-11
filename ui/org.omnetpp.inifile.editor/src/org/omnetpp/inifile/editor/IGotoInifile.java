/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor;

/**
 * Interface to allow selection of an inifile entry or section.
 * InifileEditor implements this.
 *
 * @author andras
 */
public interface IGotoInifile {
        enum Mode {AUTO, FORM, TEXT};

        /**
         * Shows the specified section in the editor. The editor will be activated
         * (focused) if mode is FORM or TEXT, but not if it is AUTO.
         *
         * @param section  may be null (to mean deselect)
         */
        public void gotoSection(String section, Mode mode);

        /**
         * Shows the specified entry in the editor. The editor will be activated
         * (focused) if mode is FORM or TEXT, but not if it is AUTO.
         *
         * @param section  may be null (to mean deselect)
         * @param key  may be null (to mean deselect)
         */
        public void gotoEntry(String section, String key, Mode mode);
}
