/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

//XXX possibly remove it; we currently don't use resource bundles
public class InifileEditorMessages {

    private static final String RESOURCE_BUNDLE= InifileEditorMessages.class.getName();

    private static ResourceBundle fgResourceBundle= ResourceBundle.getBundle(RESOURCE_BUNDLE);

    private InifileEditorMessages() {
    }

    public static String getString(String key) {
        try {
            return fgResourceBundle.getString(key);
        } catch (MissingResourceException e) {
            return "!" + key + "!";//$NON-NLS-2$ //$NON-NLS-1$
        }
    }

    public static ResourceBundle getResourceBundle() {
        return fgResourceBundle;
    }
}
