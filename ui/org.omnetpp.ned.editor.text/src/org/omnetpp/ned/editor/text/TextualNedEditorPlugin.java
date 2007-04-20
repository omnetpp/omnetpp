package org.omnetpp.ned.editor.text;

import java.io.IOException;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.text.templates.ContextTypeRegistry;
import org.eclipse.jface.text.templates.persistence.TemplateStore;
import org.eclipse.ui.editors.text.templates.ContributionContextTypeRegistry;
import org.eclipse.ui.editors.text.templates.ContributionTemplateStore;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.osgi.framework.BundleContext;

/**
 * Plugin implementing text based NED editor for OMNeT++.
 * 
 */
public class TextualNedEditorPlugin extends AbstractUIPlugin {
	
    public static String PLUGIN_ID;

    /** Key to store custom templates. */
    private static final String CUSTOM_TEMPLATES_KEY = "org.omnetpp.ned.editor.text.customtemplates"; //$NON-NLS-1$
	
	private static TextualNedEditorPlugin plugin;
	//private NedContentAssistPartitionScanner fPartitionScanner;
	
    /** The template store. */
    private TemplateStore fStore;
    /** The context type registry. */
    private ContributionContextTypeRegistry fRegistry;

	/**
	 * Creates a new plugin instance.
	 * 
	 * @param descriptor
	 */
    public TextualNedEditorPlugin() {
        super();
        plugin= this;
    }
	
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
    }
    
    
	/**
	 * Returns the default plugin instance.
	 * 
	 * @return the default plugin instance
	 */
	public static TextualNedEditorPlugin getDefault() {
		return plugin;
	}
	

     /**
     * Returns this plug-in's template store.
     * 
     * @return the template store of this plug-in instance
     */
    public TemplateStore getTemplateStore() {
        if (fStore == null) {
            fStore= new ContributionTemplateStore(getContextTypeRegistry(), getPreferenceStore(), CUSTOM_TEMPLATES_KEY);
            try {
                fStore.load();
            } catch (IOException e) {
                getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, IStatus.OK, "", e)); //$NON-NLS-1$ //$NON-NLS-2$
            }
        }
        return fStore;
    }

    /**
     * Returns this plug-in's context type registry.
     * 
     * @return the context type registry for this plug-in instance
     */
    public ContextTypeRegistry getContextTypeRegistry() {
        if (fRegistry == null) {
            // create an configure the contexts available in the template editor
            fRegistry= new ContributionContextTypeRegistry();
            fRegistry.addContextType(NedCompletionHelper.DEFAULT_NED_CONTEXT_TYPE);
        }
        return fRegistry;
    }

    public static ImageDescriptor imageDescriptorFromPlugin(String image_path) {
        return AbstractUIPlugin.imageDescriptorFromPlugin(PLUGIN_ID, image_path);
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
            exception.printStackTrace();
        }
    }
}
