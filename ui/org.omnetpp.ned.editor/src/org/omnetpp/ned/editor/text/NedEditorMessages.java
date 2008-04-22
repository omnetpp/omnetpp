package org.omnetpp.ned.editor.text;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NedEditorMessages {

	private static final String RESOURCE_BUNDLE= NedEditorMessages.class.getName();

	private static ResourceBundle fgResourceBundle= ResourceBundle.getBundle(RESOURCE_BUNDLE);

	private NedEditorMessages() {
	}

	public static String getString(String key) {
		try {
			return fgResourceBundle.getString(key);
		} catch (MissingResourceException e) {
			return "!" + key + "!"; 
		}
	}
	
	public static ResourceBundle getResourceBundle() {
		return fgResourceBundle;
	}
}
