package freemarker.eclipse.editors;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

/**
 * Access to the resource bundle of the editor
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class EditorMessages {
	private static final String RESOURCE_BUNDLE= EditorMessages.class.getName();
	private static ResourceBundle resourceBundle= ResourceBundle.getBundle(RESOURCE_BUNDLE);

	private EditorMessages() {
	}

	public static String getString(String key) {
		try {
			return resourceBundle.getString(key);
		} catch (MissingResourceException e) {
			return "!" + key + "!"; 
		}
	}

	public static ResourceBundle getResourceBundle() {
		return resourceBundle;
	}
}
