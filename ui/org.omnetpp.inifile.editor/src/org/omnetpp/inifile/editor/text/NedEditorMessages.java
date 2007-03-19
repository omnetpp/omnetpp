package org.omnetpp.inifile.editor.text;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

//XXX TODO rename, revise, possibly remove...
public class NedEditorMessages {

	private static final String RESOURCE_BUNDLE= NedEditorMessages.class.getName();//$NON-NLS-1$

	private static ResourceBundle fgResourceBundle= ResourceBundle.getBundle(RESOURCE_BUNDLE);

	private NedEditorMessages() {
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
