package org.omnetpp.msg.editor;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class MsgEditorMessages {

	private static final String RESOURCE_BUNDLE= MsgEditorMessages.class.getName();

	private static ResourceBundle fgResourceBundle= ResourceBundle.getBundle(RESOURCE_BUNDLE);

	private MsgEditorMessages() {
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
