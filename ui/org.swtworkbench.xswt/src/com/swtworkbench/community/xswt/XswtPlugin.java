package com.swtworkbench.community.xswt;

import com.swtworkbench.community.xswt.metalogger.EclipseLogger;
import com.swtworkbench.community.xswt.metalogger.ILogger;
import com.swtworkbench.community.xswt.metalogger.Logger;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

public class XswtPlugin extends AbstractUIPlugin {
    private static XswtPlugin plugin;
    private ResourceBundle resourceBundle;

    public XswtPlugin() {
        plugin = this;
        try {
            this.resourceBundle = ResourceBundle.getBundle("com.swtworkbench.community.xswt.XswtPluginResources");
        }
        catch (MissingResourceException localMissingResourceException) {
            this.resourceBundle = null;
        }
        Logger.setLogger(new EclipseLogger(this));
        Logger.log().setDebug(false);
    }

    public void start(BundleContext context) throws Exception {
        super.start(context);
    }

    public void stop(BundleContext context) throws Exception {
        super.stop(context);
    }

    public static XswtPlugin getDefault() {
        return plugin;
    }

    public static String getResourceString(String key) {
        ResourceBundle bundle = getDefault().getResourceBundle();
        try {
            return ((bundle != null) ? bundle.getString(key) : key);
        }
        catch (MissingResourceException localMissingResourceException) {
        }
        return key;
    }

    public ResourceBundle getResourceBundle() {
        return this.resourceBundle;
    }
}