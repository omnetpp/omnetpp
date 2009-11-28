/**
 *
 */
package org.omnetpp.common.wizard;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.runtime.CoreException;
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
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;

/**
 * Wizard page for selecting an IContentTemplate. Templates appear in a tree,
 * organized by categories. Templates that have "" or null as category will
 * be placed on the top level of the tree.
 *
 * @author Andras
 */
public class TemplateSelectionPage extends WizardPage {
    public static final Image DEFAULT_IMAGE = CommonPlugin.getImage("icons/obj16/wiztemplate.png");
    public static final Image CATEGORY_IMAGE = CommonPlugin.getImage("icons/obj16/wiztemplatecategory.png");

    private TreeViewer treeViewer;
    private Image defaultImage = DEFAULT_IMAGE;
    private ITemplateAddedListener templateAddedListener;
    private static String lastUrlEntered = "http://";

    /**
     * Listener to be provided when the wizard wants to provide "Load template from URL"
     * functionality. Method is supposed to call back setTemplates() to refresh the tree.
     */
    public interface ITemplateAddedListener {
        void addTemplateFrom(URL url) throws CoreException;
    }

    public TemplateSelectionPage() {
        super("OmnetppTemplateSelectionPage");
        setTitle("Initial Contents");
        setDescription("Select one of the options below");
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

    /**
     * Enable the 'Add template by URL' link, and set the callback to be invoked
     * when the user adds a template URL. Must be called before createControl().
     */
    public void setTemplateAddedListener(ITemplateAddedListener templateAddedListener) {
        if (getControl() != null)
            throw new IllegalStateException("Oh dear, too late...");
        this.templateAddedListener = templateAddedListener;
    }

    public ITemplateAddedListener getTemplateAddedListener() {
        return templateAddedListener;
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
                    String description = ((IContentTemplate)element).getDescription();
                    if (description != null)
                        return HoverSupport.addHTMLStyleSheet(description);
                }
                return null;
            }
        });

        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                TemplateSelectionPage.this.selectionChanged();
            }});

        if (templateAddedListener != null) {
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

        setPageComplete(false);
    }

    protected void selectionChanged() {
        setPageComplete(getSelectedTemplate()!=null);
    }

    protected void addTemplateByURL() {
        if (templateAddedListener == null)
            return;
        InputDialog dialog = new InputDialog(getShell(), "Enter Template URL", "Wizard template URL (should point to the folder containing template.properties)", lastUrlEntered, null);
        if (dialog.open() == Dialog.OK && !StringUtils.isBlank(dialog.getValue())) {
            String url = dialog.getValue().trim();
            lastUrlEntered = url;
            try {
                templateAddedListener.addTemplateFrom(new URL(url));
            }
            catch (MalformedURLException e) {
                MessageDialog.openError(getShell(), "Error", "Malformed URL '" + url + "': " + StringUtils.defaultString(e.getMessage()) + '.');
            }
            catch (CoreException e) {
                ErrorDialog.openError(getShell(), "Error", "Could not add template at " + url, e.getStatus());
            }
        }
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
}
