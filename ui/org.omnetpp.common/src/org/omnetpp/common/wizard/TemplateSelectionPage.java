/**
 *
 */
package org.omnetpp.common.wizard;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.ViewerComparator;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.osgi.framework.Bundle;

/**
 * Wizard page for selecting an IContentTemplate. Templates appear in a tree,
 * organized by categories. Templates that have "" or null as category will
 * be placed on the top level of the tree.
 *
 * @author Andras
 */
public class TemplateSelectionPage extends WizardPage {
    public static final String TEMPLATES_FOLDER_NAME = "templates";

    public static final String CONTENTTEMPLATE_EXTENSIONPOINT_ID = "org.omnetpp.common.wizard.contenttemplates";
    public static final String PLUGIN_ELEMENT = "plugin";
    public static final String PLUGINID_ATT = "pluginId";
    public static final String FOLDER_ATT = "folder";

    protected static final Image DEFAULT_IMAGE = CommonPlugin.getImage("icons/obj16/wiztemplate.png");
    protected static final Image CATEGORY_IMAGE = CommonPlugin.getImage("icons/obj16/wiztemplatecategory.png");

    private String wizardType;
    private boolean enableAddLink;
    private List<IContentTemplate> dynamicallyAddedTemplates = new ArrayList<IContentTemplate>();
    private TreeViewer treeViewer;
    private Image defaultImage = DEFAULT_IMAGE;

    private static String lastUrlEntered = "http://";

    /**
     * Creates the template selection page. The wizardType parameter is used for filtering the
     * offered templates; it can be null to show all templates. 
     */
    public TemplateSelectionPage(String wizardType, boolean enableAddLink) {
        super("OmnetppTemplateSelectionPage");
        setTitle("Initial Contents");
        setDescription("Select one of the options below");
        this.wizardType = wizardType;
        this.enableAddLink = enableAddLink;
    }

    public String getWizardType() {
        return wizardType;
    }

    /**
     * Returns the image to be used for templates that don't have one.
     */
    public Image getDefaultImage() {
        return defaultImage;
    }

    /**
     * Sets the image to be used for templates that don't have one.
     */
    public void setDefaultImage(Image defaultImage) {
        this.defaultImage = defaultImage;
    }

    public void createControl(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayout(new GridLayout(1,false));
        setControl(composite);

        // create tree and label
        Label label = new Label(composite, SWT.NONE);
        label.setText("&Select template:");
        treeViewer = new TreeViewer(composite, SWT.BORDER);
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)treeViewer.getTree().getLayoutData()).heightHint = 200;

        treeViewer.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                element = ((GenericTreeNode)element).getPayload();
                return element instanceof IContentTemplate ? ((IContentTemplate)element).getName() : element.toString();
            }
            @Override
            public Image getImage(Object element) {
                element = ((GenericTreeNode)element).getPayload();
                if (element instanceof IContentTemplate) {
                    Image image = ((IContentTemplate)element).getImage();
                    return image!=null ? image : defaultImage;
                }
                else {
                    return CATEGORY_IMAGE;
                }
            }
        });

        treeViewer.setContentProvider(new GenericTreeContentProvider());

        // set ordering: default templates first, then non-default templates, then template categories
        treeViewer.setComparator(new ViewerComparator() {
            @Override
            public int category(Object element) {
                element = ((GenericTreeNode)element).getPayload();
                if (element instanceof IContentTemplate)
                    return ((IContentTemplate) element).getIsDefault() ? 1 : 2;
                if (element instanceof String)  // i.e. category name
                    return 3;
                return 4; // cannot happen
            }
        });

        // show the descriptions in a tooltip
        new HoverSupport().adapt(treeViewer.getTree(), new IHoverTextProvider() {
            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
                Item item = treeViewer.getTree().getItem(new Point(x,y));
                Object element = item==null ? null : item.getData();
                element = (element instanceof GenericTreeNode) ? ((GenericTreeNode)element).getPayload() : null;
                if (element instanceof IContentTemplate) {
                    String description = getTemplateHoverText((IContentTemplate)element);
                    if (description != null)
                        return HoverSupport.addHTMLStyleSheet(description);
                }
                return null;
            }
        });

        // configure treeviewer behavior
        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                TemplateSelectionPage.this.selectionChanged();
            }});

        treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                Object elem = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
                if (getSelectedTemplate() != null)
                    templateSelectionMade();  // accept double-clicked template
                else
                    treeViewer.setExpandedState(elem, !treeViewer.getExpandedState(elem)); // open/close category
            }
        });
        
        // add "add content template" link
        if (enableAddLink) {
            Link link = new Link(composite, SWT.NONE);
            link.setText("<a>Add content template by URL</a>");
            link.addSelectionListener(new SelectionListener() {
                public void widgetDefaultSelected(SelectionEvent e) {
                    addTemplateByURL();
                }

                public void widgetSelected(SelectionEvent e) {
                    addTemplateByURL();
                }});
        }

        // fill the page
        List<IContentTemplate> templates = getTemplates();
        setTemplates(templates);
        setSelectedTemplate(getRememberedTemplate(templates));

        setPageComplete(false);
    }

    protected void selectionChanged() {
        setPageComplete(getSelectedTemplate()!=null);
    }

    protected String getTemplateHoverText(IContentTemplate template) {
        return template.getDescription();
    }

    public void setTemplates(List<IContentTemplate> templates) {
        GenericTreeNode root = new GenericTreeNode("root");
        Set<String> categories = new LinkedHashSet<String>();
        for (IContentTemplate template : templates)
            categories.add(template.getCategory());
        for (String category : categories) {
            GenericTreeNode categoryNode = StringUtils.isEmpty(category) ? root : new GenericTreeNode(category);
            if (categoryNode != root)
                root.addChild(categoryNode);
            for (IContentTemplate template : templates)
                if (StringUtils.equals(category, template.getCategory()))
                    categoryNode.addChild(new GenericTreeNode(template));
        }
        treeViewer.setInput(root);

        if (!templates.isEmpty()) {
            // preselect the (first) template marked as default
            IContentTemplate defaultTemplate = null;
            for (IContentTemplate template : templates)
                if (template.getIsDefault()) {defaultTemplate = template; break; }
            if (defaultTemplate == null && !templates.isEmpty())
                defaultTemplate = templates.get(0);

            treeViewer.setSelection(new StructuredSelection(new GenericTreeNode(defaultTemplate)));
        }

        selectionChanged(); // update page state
    }

    public IContentTemplate getSelectedTemplate() {
        Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        element = (element == null) ? null : ((GenericTreeNode)element).getPayload();
        return (element instanceof IContentTemplate) ? (IContentTemplate)element : null;
    }

    public void setSelectedTemplate(IContentTemplate template) {
        if (template == null)
            treeViewer.setSelection(new StructuredSelection());
        else {
            Object input = treeViewer.getInput();
            GenericTreeNode node = ((GenericTreeNode)input).findPayload(template);
            if (node != null)
                treeViewer.setSelection(new StructuredSelection(node), true);
        }
    }

    /**
     * Called when a template was double-clicked
     */
    protected void templateSelectionMade() {
        if (canFlipToNextPage())
            getContainer().showPage(getNextPage());
    }

    protected IContentTemplate getRememberedTemplate(List<IContentTemplate> templates) {
        String ident = getDialogSettings().get(getWizardType()+".template");
        for (IContentTemplate template : templates)
            if (template.getIdentifierString().equals(ident))
                return template;
        return null;
    }

    protected void addTemplateByURL() {
        InputDialog dialog = new InputDialog(getShell(), "Enter Template URL", "Wizard template URL (should point to the folder containing template.properties)", lastUrlEntered, null);
        if (dialog.open() == Dialog.OK && !StringUtils.isBlank(dialog.getValue())) {
            String url = dialog.getValue().trim();
            lastUrlEntered = url;
            try {
                addTemplateFrom(new URL(url));
            }
            catch (MalformedURLException e) {
                MessageDialog.openError(getShell(), "Error", "Malformed URL '" + url + "': " + StringUtils.defaultString(e.getMessage()) + '.');
            }
            catch (CoreException e) {
                ErrorDialog.openError(getShell(), "Error", "Could not add template at " + url, e.getStatus());
            }
        }
    }

    protected void addTemplateFrom(URL url) throws CoreException {
        IContentTemplate template = loadTemplateFromURL(url, null);
        boolean ok = getWizardType()==null || template.getSupportedWizardTypes().isEmpty() || template.getSupportedWizardTypes().contains(getWizardType());
        ok = ok && isSuitableTemplate(template);
        if (!ok) {
            MessageDialog.openError(getShell(), "Error", 
                    "The provided content template does not support this wizard dialog; " + 
                    "try in other dialogs. (The template supports the following wizard types: " + 
                    StringUtils.join(template.getSupportedWizardTypes(), ", ") + ")");
        }
        else {
            dynamicallyAddedTemplates.add(template);
            setTemplates(getTemplates()); // refresh page
            setSelectedTemplate(template);
        }
    }

    /**
     * Return the templates to be shown on the page. This implementation
     * returns the templates that match the wizards' type (see getWizardType()).
     * Override this method to add more templates, or to introduce additional
     * filtering (i.e. by options the user selected on previous wizard pages,
     * such as the "With C++ Support" checkbox in the New OMNeT++ Project wizard).
     */
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        result.addAll(loadBuiltinTemplates(getWizardType()));
        result.addAll(loadTemplatesFromWorkspace(getWizardType()));
        result.addAll(dynamicallyAddedTemplates);
        //TODO define an ITemplateSource interface and a matching extension point
        return result;
    }

    /**
     * Loads built-in templates from all plug-ins registered via the
     * org.omnetpp.common.wizard.contenttemplates extension point.
     */
    protected List<IContentTemplate> loadBuiltinTemplates(final String wizardType) {
        final List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        try {
            IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(CONTENTTEMPLATE_EXTENSIONPOINT_ID);
            for (IConfigurationElement e : config) {
                Assert.isTrue(e.getName().equals(PLUGIN_ELEMENT));
                final String pluginName = e.getAttribute(PLUGINID_ATT);
                final String folderName = StringUtils.defaultIfEmpty(e.getAttribute(FOLDER_ATT), TEMPLATES_FOLDER_NAME);
                ISafeRunnable runnable = new ISafeRunnable() {
                    public void run() throws Exception {
                        Bundle bundle = Platform.getBundle(pluginName);
                        if (bundle == null)
                            throw new RuntimeException("Cannot resolve bundle " + pluginName);
                        else
                            result.addAll(loadBuiltinTemplates(bundle, folderName, wizardType));
                    }
                    public void handleException(Throwable e) {
                        CommonPlugin.logError("Cannot read templates from plug-in " + pluginName, e);
                    }
                };
                SafeRunner.run(runnable);
            }
        } catch (Exception ex) {
            CommonPlugin.logError("Error loading built-in templates from plug-ins", ex);
        }
        return result;
    }

    /**
     * Loads built-in templates that support the given wizard type from the given plugin.
     */
    @SuppressWarnings("unchecked")
    protected List<IContentTemplate> loadBuiltinTemplates(Bundle bundle, String folderName, String wizardType) {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        try {
            Enumeration e = bundle.findEntries(folderName, FileBasedContentTemplate.TEMPLATE_PROPERTIES_FILENAME, true);
            if (e != null) {
                while (e.hasMoreElements()) {
                    URL propFileUrl = (URL) e.nextElement();
                    URL templateUrl = new URL(StringUtils.removeEnd(propFileUrl.toString(), FileBasedContentTemplate.TEMPLATE_PROPERTIES_FILENAME));
                    IContentTemplate template = loadTemplateFromURL(templateUrl, bundle);
                    if (wizardType==null || template.getSupportedWizardTypes().isEmpty() || template.getSupportedWizardTypes().contains(wizardType))
                        if (isSuitableTemplate(template))
                            result.add(template);
                }
            }
        } catch (Exception e) {
            CommonPlugin.logError("TemplateBasedWizard: Could not load content templates from plug-ins", e);
        }
        return result;
    }

    /**
     * Factory method used by loadBuiltinTemplates(); override if you subclass
     * from FileBasedContentTemplate. bundleOfTemplate may be null.
     */
    protected IContentTemplate loadTemplateFromURL(URL templateUrl, Bundle bundleOfTemplate) throws CoreException {
        return new FileBasedContentTemplate(templateUrl, bundleOfTemplate);
    }

    /**
     * Utility method for getTemplates()
     */
    protected List<IContentTemplate> loadTemplatesFromWorkspace(String wizardType) {
        // check the "templates/" subdirectory of each OMNeT++ project
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        for (IProject project : ProjectUtils.getOmnetppProjects()) {
            IFolder rootFolder = project.getFolder(new Path(TEMPLATES_FOLDER_NAME));
            if (rootFolder.exists()) {
                try {
                    // each template is a folder which contains a "template.properties" file
                    for (IResource resource : rootFolder.members()) {
                        if (resource instanceof IFolder && FileBasedContentTemplate.looksLikeTemplateFolder((IFolder)resource)) {
                            IFolder folder = (IFolder)resource;
                            IContentTemplate template = loadTemplateFromWorkspace(folder);
                            if (wizardType==null || template.getSupportedWizardTypes().isEmpty() || template.getSupportedWizardTypes().contains(wizardType))
                                if (isSuitableTemplate(template))
                                    result.add(template);
                        }
                    }
                } catch (CoreException e) {
                    CommonPlugin.logError("Error loading project templates from " + rootFolder.toString(), e);
                }
            }
        }
        return result;
    }

    /**
     * Override for further filtering of templates (beyond wizardType). The default 
     * implementation just returns true. 
     */
    protected boolean isSuitableTemplate(IContentTemplate template) {
        return true;
    }

    /**
     * Factory method used by loadTemplatesFromWorkspace(); override if you subclass
     * from FileBasedContentTemplate.
     */
    protected IContentTemplate loadTemplateFromWorkspace(IFolder folder) throws CoreException {
        return new FileBasedContentTemplate(folder);
    }


}
