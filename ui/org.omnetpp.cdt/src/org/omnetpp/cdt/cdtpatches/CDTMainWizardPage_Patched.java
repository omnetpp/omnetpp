package org.omnetpp.cdt.cdtpatches;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import org.eclipse.cdt.internal.ui.CPluginImages;
import org.eclipse.cdt.ui.newui.CDTPrefUtil;
import org.eclipse.cdt.ui.newui.PageLayout;
import org.eclipse.cdt.ui.newui.ProjectContentsArea;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.newui.ProjectContentsArea.IErrorMessageReporter;
import org.eclipse.cdt.ui.wizards.CDTMainWizardPage;
import org.eclipse.cdt.ui.wizards.CNewWizard;
import org.eclipse.cdt.ui.wizards.CWizardHandler;
import org.eclipse.cdt.ui.wizards.EntryDescriptor;
import org.eclipse.cdt.ui.wizards.IWizardItemsListListener;
import org.eclipse.cdt.ui.wizards.IWizardWithMemory;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.IExtension;
import org.eclipse.core.runtime.IExtensionPoint;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.DialogPage;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.accessibility.AccessibleAdapter;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.PlatformUI;

/**
 * This is a complete copy of CDTMainWizardPage, with some things modified.
 * The original could not be reused gently because of too much "private" and "static"
 * inside, hence the brute force applied.
 * 
 * Don't blame me how the code looks -- blame the CDT guys.
 * 
 * Modifications marked with (AV).
 * 
 * See also: https://bugs.eclipse.org/bugs/show_bug.cgi?id=211935  
 *
 * @author Andras
 */
public class CDTMainWizardPage_Patched extends CDTMainWizardPage {
    private static final Image IMG_CATEGORY = CPluginImages.get(CPluginImages.IMG_OBJS_SEARCHFOLDER);
    private static final Image IMG_ITEM = CPluginImages.get(CPluginImages.IMG_OBJS_VARIABLE);

    //(AV) public static final String PAGE_ID = "org.eclipse.cdt.managedbuilder.ui.wizard.NewModelProjectWizardPage"; //$NON-NLS-1$

    private static final String EXTENSION_POINT_ID = "org.eclipse.cdt.ui.CDTWizard"; //$NON-NLS-1$
    private static final String ELEMENT_NAME = "wizard"; //$NON-NLS-1$
    private static final String CLASS_NAME = "class"; //$NON-NLS-1$
    private static final String HELP_CTX = "org.eclipse.ui.ide.new_project_wizard_page_context"; //$NON-NLS-1$
    //(AV) public static final String DESC = "EntryDescriptor"; //$NON-NLS-1$ 
    // constants
    private static final int SIZING_TEXT_FIELD_WIDTH = 250;

    // initial value stores
    protected /*(AV) private*/ String initialProjectFieldValue;

    // widgets
    protected /*(AV) private*/ Text projectNameField;
    private Tree tree;
    private Composite right;
    private Button show_sup;
    private Label right_label;

    protected /*(AV) private*/ ProjectContentsArea locationArea;
    //(AV) public CWizardHandler h_selected = null;

    /**
     * Creates a new project creation wizard page.
     *
     * @param pageName the name of this page
     */
    public CDTMainWizardPage_Patched(String pageName) {
        super(pageName);
    }

    /** (non-Javadoc)
     * Method declared on IDialogPage.
     */
    public void createControl(Composite parent) {
        Composite composite = new Composite(parent, SWT.NULL);
        composite.setFont(parent.getFont());

        initializeDialogUnits(parent);
        PlatformUI.getWorkbench().getHelpSystem().setHelp(composite, HELP_CTX);

        composite.setLayout(new GridLayout());
        composite.setLayoutData(new GridData(GridData.FILL_BOTH));

        createProjectNameGroup(composite);
        locationArea = new ProjectContentsArea(getErrorReporter(), composite);
        if(initialProjectFieldValue != null) {
            locationArea.updateProjectName(initialProjectFieldValue);
        }

        // Scale the button based on the rest of the dialog
        setButtonLayoutData(locationArea.getBrowseButton());

        createDynamicGroup(composite); 

        switchTo(updateData_AV(tree, right, show_sup, CDTMainWizardPage_Patched.this, getWizard()),
                getDescriptor(tree));

        setPageComplete(validatePage());
        // Show description on opening
        setErrorMessage(null);
        setMessage(null);
        setControl(composite);
    }

    private void createDynamicGroup(Composite parent) {
        Composite c = new Composite(parent, SWT.NONE);
        c.setLayoutData(new GridData(GridData.FILL_BOTH));
        c.setLayout(new GridLayout(2, true));

        Label l1 = new Label(c, SWT.NONE);
        l1.setText(UIMessages.getString("CMainWizardPage.0")); //$NON-NLS-1$
        l1.setFont(parent.getFont());
        l1.setLayoutData(new GridData(GridData.BEGINNING));

        right_label = new Label(c, SWT.NONE);
        right_label.setFont(parent.getFont());
        right_label.setLayoutData(new GridData(GridData.BEGINNING));

        tree = new Tree(c, SWT.SINGLE | SWT.BORDER);
        tree.setLayoutData(new GridData(GridData.FILL_BOTH));
        tree.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                TreeItem[] tis = tree.getSelection();
                if (tis == null || tis.length == 0) return;
                switchTo((CWizardHandler)tis[0].getData(), (EntryDescriptor)tis[0].getData(DESC));
                setPageComplete(validatePage());
            }});
        tree.getAccessible().addAccessibleListener(
                new AccessibleAdapter() {                       
                    public void getName(AccessibleEvent e) {
                        e.result = UIMessages.getString("CMainWizardPage.0"); //$NON-NLS-1$
                    }
                }
        );
        right = new Composite(c, SWT.NONE);
        right.setLayoutData(new GridData(GridData.FILL_BOTH));
        right.setLayout(new PageLayout());

        show_sup = new Button(c, SWT.CHECK);
        show_sup.setText(UIMessages.getString("CMainWizardPage.1")); //$NON-NLS-1$
        GridData gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = 2;
        show_sup.setLayoutData(gd);
        show_sup.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                if (h_selected != null)
                    h_selected.setSupportedOnly(show_sup.getSelection());
                switchTo(updateData_AV(tree, right, show_sup, CDTMainWizardPage_Patched.this, getWizard()),
                        getDescriptor(tree));
            }} );

        // restore settings from preferences
        show_sup.setSelection(!CDTPrefUtil.getBool(CDTPrefUtil.KEY_NOSUPP));
    }

    /**
     * Get an error reporter for the receiver.
     * @return IErrorMessageReporter
     */
    private IErrorMessageReporter getErrorReporter() {
        return new IErrorMessageReporter(){
            public void reportError(String errorMessage) {
                setErrorMessage(errorMessage);
                boolean valid = errorMessage == null;
                if(valid) valid = validatePage();
                setPageComplete(valid);
            }
        };
    }

    public IWizardPage getNextPage() {
        if (h_selected == null) // cannot continue
            return null;
        return h_selected.getSpecificPage();
    }       
    /**
     * Creates the project name specification controls.
     *
     * @param parent the parent composite
     */
    private final void createProjectNameGroup(Composite parent) {
        // project specification group
        Composite projectGroup = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout();
        layout.numColumns = 2;
        projectGroup.setLayout(layout);
        projectGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        // new project label
        Label projectLabel = new Label(projectGroup, SWT.NONE);
        projectLabel.setText(UIMessages.getString("CMainWizardPage.8")); //$NON-NLS-1$
        projectLabel.setFont(parent.getFont());

        // new project name entry field
        projectNameField = new Text(projectGroup, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_HORIZONTAL);
        data.widthHint = SIZING_TEXT_FIELD_WIDTH;
        projectNameField.setLayoutData(data);
        projectNameField.setFont(parent.getFont());

        // Set the initial value first before listener
        // to avoid handling an event during the creation.
        if (initialProjectFieldValue != null) {
            projectNameField.setText(initialProjectFieldValue);
        }
        projectNameField.addListener(SWT.Modify, new Listener() {
            public void handleEvent(Event e) {
                locationArea.updateProjectName(getProjectNameFieldValue());
                setPageComplete(validatePage());
            }
        });
    }

    /**
     * Creates a project resource handle for the current project name field value.
     * <p>
     * This method does not create the project resource; this is the responsibility
     * of <code>IProject::create</code> invoked by the new project resource wizard.
     * </p>
     *
     * @return the new project resource handle
     */
    private IProject getProjectHandle() {
        return ResourcesPlugin.getWorkspace().getRoot().getProject(getProjectName());
    }

    /**
     * Returns the current project name as entered by the user, or its anticipated
     * initial value.
     *
     * @return the project name, its anticipated initial value, or <code>null</code>
     *   if no project name is known
     */

    public String getProjectName() {
        if (projectNameField == null) {
            return initialProjectFieldValue;
        }
        return getProjectNameFieldValue();
    }

    public IPath getProjectLocation() {
        if (locationArea.isDefault()) return null;
        return new Path(locationArea.getProjectLocation());
    }

    public String getProjectLocationPath() {
        return locationArea.getProjectLocation();
    }

    /**
     * Returns the value of the project name field
     * with leading and trailing spaces removed.
     * 
     * @return the project name in the field
     */
    private String getProjectNameFieldValue() {
        if (projectNameField == null) {
            return ""; //$NON-NLS-1$
        }

        return projectNameField.getText().trim();
    }

    /**
     * Sets the initial project name that this page will use when
     * created. The name is ignored if the createControl(Composite)
     * method has already been called. Leading and trailing spaces
     * in the name are ignored.
     * Providing the name of an existing project will not necessarily 
     * cause the wizard to warn the user.  Callers of this method 
     * should first check if the project name passed already exists 
     * in the workspace.
     * 
     * @param name initial project name for this page
     * 
     * @see IWorkspace#validateName(String, int)
     * 
     */
    public void setInitialProjectName(String name) {
        if (name == null) {
            initialProjectFieldValue = null;
        } else {
            initialProjectFieldValue = name.trim();
            if(locationArea != null) {
                locationArea.updateProjectName(name.trim());
            }
        }
    }

    /**
     * Returns whether this page's controls currently all contain valid 
     * values.
     *
     * @return <code>true</code> if all controls are valid, and
     *   <code>false</code> if at least one is invalid
     */
    protected boolean validatePage() {
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        setMessage(null);

        String projectFieldContents = getProjectNameFieldValue();
        if (projectFieldContents.length() == 0) {
            setErrorMessage(UIMessages.getString("CMainWizardPage.9")); //$NON-NLS-1$
            return false;
        }

        IStatus nameStatus = workspace.validateName(projectFieldContents,
                IResource.PROJECT);
        if (!nameStatus.isOK()) {
            setErrorMessage(nameStatus.getMessage());
            return false;
        }

        if (projectFieldContents.indexOf('#') >= 0) {
            setErrorMessage(UIMessages.getString("CDTMainWizardPage.0"));                //$NON-NLS-1$
            return false;
        }


        boolean bad = true; // should we treat existing project as error

        IProject handle = getProjectHandle();
        if (handle.exists()) {
            if (getWizard() instanceof IWizardWithMemory) {
                IWizardWithMemory w = (IWizardWithMemory)getWizard();
                if (w.getLastProjectName() != null && w.getLastProjectName().equals(getProjectName()))
                    bad = false;
            }
            if (bad) { 
                setErrorMessage(UIMessages.getString("CMainWizardPage.10")); //$NON-NLS-1$
                return false;
            }
        }

        if (bad) { // skip this check if project already created 
            IPath p = getProjectLocation();
            if (p == null) {
                p = ResourcesPlugin.getWorkspace().getRoot().getLocation();
                p = p.append(getProjectName());
            }
            File f = p.toFile();
            if (f.exists()) {
                if (f.isDirectory()) {
                    setMessage(UIMessages.getString("CMainWizardPage.7"), DialogPage.WARNING); //$NON-NLS-1$
                    return true;
                } else {
                    setErrorMessage(UIMessages.getString("CMainWizardPage.6")); //$NON-NLS-1$
                    return false;
                }
            }
        }

        if (!locationArea.isDefault()) {
            IStatus locationStatus = workspace.validateProjectLocationURI(handle,
                    locationArea.getProjectLocationURI());
            if (!locationStatus.isOK()) {
                setErrorMessage(locationStatus.getMessage());
                return false;
            }
        }

        if (tree.getItemCount() == 0) {
            setErrorMessage(UIMessages.getString("CMainWizardPage.3")); //$NON-NLS-1$
            return false;
        }

        // it is not an error, but we cannot continue
        if (h_selected == null) {
            setErrorMessage(null);
            return false;               
        }

        String s = h_selected.getErrorMessage(); 
        if (s != null) {
            setErrorMessage(s);
            return false;
        }

        setErrorMessage(null);
        return true;
    }

    /*
     * see @DialogPage.setVisible(boolean)
     */
    public void setVisible(boolean visible) {
        //(AV) super.setVisible(visible);
        getControl().setVisible(visible);  //(AV)
        if (visible) projectNameField.setFocus();
    }

    /**
     * Returns the useDefaults.
     * @return boolean
     */
    public boolean useDefaults() {
        return locationArea.isDefault();
    }

    protected /*public static*/ CWizardHandler updateData_AV(Tree tree, Composite right, Button show_sup, IWizardItemsListListener ls, IWizard wizard) {
        // remember selected item
        TreeItem[] sel = tree.getSelection();
        String savedStr = (sel.length > 0) ? sel[0].getText() : null; 

        tree.removeAll();
        IExtensionPoint extensionPoint = Platform.getExtensionRegistry()
        .getExtensionPoint(EXTENSION_POINT_ID);
        if (extensionPoint == null) return null;
        IExtension[] extensions = extensionPoint.getExtensions();
        if (extensions == null) return null;

        ArrayList items = new ArrayList();
        for (int i = 0; i < extensions.length; ++i) {
            IConfigurationElement[] elements = extensions[i].getConfigurationElements();
            for (int k = 0; k < elements.length; k++) {
                if (elements[k].getName().equals(ELEMENT_NAME)) {
                    CNewWizard w = null;
                    try {
                        w = (CNewWizard) elements[k].createExecutableExtension(CLASS_NAME);
                    } catch (CoreException e) {
                        System.out.println(UIMessages.getString("CMainWizardPage.5") + e.getLocalizedMessage()); //$NON-NLS-1$
                        return null; 
                    }
                    if (w == null) return null;

                    w.setDependentControl(right, ls);
                    EntryDescriptor[] wd = w.createItems(show_sup.getSelection(), wizard);
                    for (int x=0; x<wd.length; x++) items.add(wd[x]);
                }
            }
        }
        
        items = filterItems(items); //(AV) added
        addItemsToTree(tree, items);

        if (tree.getItemCount() > 0) {
            TreeItem target = tree.getItem(0);
            // try to search item which was selected before
            if (savedStr != null) {
                TreeItem[] all = tree.getItems();
                for (int i=0; i<all.length; i++) {
                    if (savedStr.equals(all[i].getText())) {
                        target = all[i];
                        break;
                    }
                }
            }
            tree.setSelection(target);
            return (CWizardHandler)target.getData();
        }
        return null;
    }

    //(AV) added 
    protected ArrayList<EntryDescriptor> filterItems(ArrayList<EntryDescriptor> items) {
        return items;
    }

    private static void addItemsToTree(Tree tree, ArrayList items) {
        //  Sorting is disabled because of users requests   
        //  Collections.sort(items, CDTListComparator.getInstance());

        ArrayList placedTreeItemsList = new ArrayList(items.size());
        ArrayList placedEntryDescriptorsList = new ArrayList(items.size());
        Iterator it = items.iterator();
        while (it.hasNext()) {
            EntryDescriptor wd = (EntryDescriptor)it.next();
            if (wd.getParentId() == null) {
                wd.setPath(wd.getId());
                TreeItem ti = new TreeItem(tree, SWT.NONE);
                ti.setText(wd.getName());
                ti.setData(wd.getHandler());
                ti.setData(DESC, wd);
                ti.setImage(calcImage(wd));
                placedTreeItemsList.add(ti);
                placedEntryDescriptorsList.add(wd);
            }
        }
        while(true) {
            boolean found = false;
            Iterator it2 = items.iterator();
            while (it2.hasNext()) {
                EntryDescriptor wd1 = (EntryDescriptor)it2.next();
                if (wd1.getParentId() == null) continue;
                for (int i=0; i<placedEntryDescriptorsList.size(); i++) {
                    EntryDescriptor wd2 = (EntryDescriptor)placedEntryDescriptorsList.get(i);
                    if (wd2.getId().equals(wd1.getParentId())) {
                        found = true;
                        wd1.setParentId(null);
                        CWizardHandler h = wd2.getHandler();
                        if (h == null && !wd1.isCategory()) 
                            break;

                        wd1.setPath(wd2.getPath() + "/" + wd1.getId()); //$NON-NLS-1$
                        wd1.setParent(wd2);
                        if (wd1.getHandler() == null && !wd1.isCategory())
                            wd1.setHandler((CWizardHandler)h.clone());
                        if (h != null && !h.isApplicable(wd1))
                            break;

                        TreeItem p = (TreeItem)placedTreeItemsList.get(i);
                        TreeItem ti = new TreeItem(p, SWT.NONE);
                        ti.setText(wd1.getName());
                        ti.setData(wd1.getHandler());
                        ti.setData(DESC, wd1);
                        ti.setImage(calcImage(wd1));
                        placedTreeItemsList.add(ti);
                        placedEntryDescriptorsList.add(wd1);
                        break;
                    }
                }
            }
            // repeat iterations until all items are placed.
            if (!found) break;
        }
        // orphan elements (with not-existing parentId) are ignored
    }

    private void switchTo(CWizardHandler h, EntryDescriptor ed) {
        if (h == null) 
            h = ed.getHandler();
        try {
            if (h != null && ed != null) 
                h.initialize(ed);
        } catch (CoreException e) { 
            h = null; 
        }
        if (h_selected != null) 
            h_selected.handleUnSelection();
        h_selected = h;
        if (h == null) 
            return;
        right_label.setText(h_selected.getHeader());
        h_selected.handleSelection();
        h_selected.setSupportedOnly(show_sup.getSelection());
    }


    public static EntryDescriptor getDescriptor(Tree _tree) {
        TreeItem[] sel = _tree.getSelection();
        if (sel == null || sel.length == 0) 
            return null;
        else
            return (EntryDescriptor)sel[0].getData(DESC);
    }

    public void toolChainListChanged(int count) {
        setPageComplete(validatePage());
        getWizard().getContainer().updateButtons();
    }

    public boolean isCurrent() { return isCurrentPage(); }

    private static Image calcImage(EntryDescriptor ed) {
        if (ed.getImage() != null) return ed.getImage();
        if (ed.isCategory()) return IMG_CATEGORY;
        return IMG_ITEM;
    }
}


