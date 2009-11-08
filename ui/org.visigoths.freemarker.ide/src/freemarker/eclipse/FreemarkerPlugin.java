package freemarker.eclipse;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Preferences;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;

import freemarker.eclipse.preferences.IPreferenceConstants;

/**
 * @version $Id: FreemarkerPlugin.java,v 1.6 2004/06/04 15:00:20 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller </a>
 */
public class FreemarkerPlugin extends AbstractUIPlugin implements IPreferenceConstants {
	public static final String PLUGIN_ID = "org.visigoths.freemarker.ide";

	// The shared instance.
	private static FreemarkerPlugin plugin;

	/**
	 * The constructor.
	 */
	public FreemarkerPlugin() {
		plugin = this;
	}

	/**
	 * Returns the shared instance.
	 */
	public static FreemarkerPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns the workspace instance.
	 */
	public static IWorkspace getWorkspace() {
		return ResourcesPlugin.getWorkspace();
	}

	public static FreemarkerPlugin getInstance() {
		return plugin;
	}

	/**
	 * Initializes the plugin preferences with default preference values for
	 * this plug-in.
	 */
	protected void initializeDefaultPluginPreferences() {
		Preferences prefs = getPluginPreferences();
		prefs.setDefault(SHOW_ICONS, true);
		prefs.setDefault(COLOR_COMMENT, "170,0,0");
		prefs.setDefault(COLOR_TEXT, "0,0,0");
		prefs.setDefault(COLOR_INTERPOLATION, "255,0,128");
		prefs.setDefault(COLOR_DIRECTIVE, "0,0,255");
		prefs.setDefault(COLOR_STRING, "0,128,128");
		prefs.setDefault(COLOR_XML_COMMENT, "128,128,128");
		prefs.setDefault(COLOR_XML_TAG, "0,0,128");
		prefs.setDefault(XML_HIGHLIGHTING, true);
	}
	
    public static void logError(Throwable exception) {
        logError(exception.toString(), exception);
    }

    public static void logError(String message, Throwable exception) {
        if (plugin != null) {
            plugin.getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, message, exception));
        }
        else {
            System.err.println(message);
            if (exception!=null)
                exception.printStackTrace();
        }
    }
	
}