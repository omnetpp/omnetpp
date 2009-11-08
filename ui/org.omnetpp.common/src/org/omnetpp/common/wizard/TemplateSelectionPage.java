/**
 * 
 */
package org.omnetpp.common.wizard;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
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

    public TemplateSelectionPage() {
        super("OmnetppTemplateSelectionPage");
        setTitle("Project Contents");
        setDescription("Select one of the options below");
    }

    public Image getDefaultImage() {
        return defaultImage;
    }

    public void setDefaultImage(Image defaultImage) {
        this.defaultImage = defaultImage;
    }

    public void createControl(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayout(new GridLayout(1,false));
        setControl(composite);

        // create tree and label
        Label label = new Label(composite, SWT.NONE);
        label.setText("Select template:");
        treeViewer = new TreeViewer(composite, SWT.BORDER);
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
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

        // always complete
        setPageComplete(true);
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
    }

    public IContentTemplate getSelectedTemplate() {
        Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        element = element == null ? null : ((GenericTreeNode)element).getPayload();
        return (element instanceof IContentTemplate) ? (IContentTemplate)element : null;
    }
}