/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;

import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.ui.newui.CDTPropertyManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.PatternFilter;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.ProjectFeature;
import org.omnetpp.cdt.build.ProjectFeaturesManager;
import org.omnetpp.cdt.build.ProjectFeaturesManager.Problem;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.project.NedSourceFoldersConfiguration;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.FilteredCheckboxTree;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.ui.ProblemsMessageDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;

/**
 * This property page is shown for an OMNeT++ CDT Project, and lets the user
 * enable or disable "project features".
 *
 * @author Andras
 */
public class ProjectFeaturesPropertyPage extends PropertyPage {
    // features of the edited project
    protected ProjectFeaturesManager features;

    // controls
    protected Link errorMessageLabel;
    protected FilteredCheckboxTree filteredTree;
    protected CheckboxTreeViewer treeViewer;

    protected NedSourceFoldersConfiguration nedSourceFoldersConfig;


    /**
     * Constructor.
     */
    public ProjectFeaturesPropertyPage() {
        super();
    }

    /**
     * Creates page contents
     */
    protected Control createContents(Composite parent) {
        final Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        composite.setLayout(new GridLayout(2,false));
        ((GridLayout)composite.getLayout()).marginWidth = 0;
        ((GridLayout)composite.getLayout()).marginHeight = 0;

        // info text
        String text =
            "This page allows you to disable parts (\"features\") of the selected OMNeT++ project, " +
            "for example to shorten build time. Feature definitions come from the project's " +
            "\"" + ProjectFeaturesManager.PROJECTFEATURES_FILENAME + "\" file.";
        final Label bannerTextLabel = createLabel(composite, text);
        bannerTextLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
        ((GridData)bannerTextLabel.getLayoutData()).widthHint = 300;

        // error message area
        errorMessageLabel = new Link(composite, SWT.WRAP);
        errorMessageLabel.setForeground(ColorFactory.RED2);
        errorMessageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, 2, 1));
        ((GridData)errorMessageLabel.getLayoutData()).widthHint = 300;

        PatternFilter patternFilter = new PatternFilter() {
            @Override
            protected boolean isLeafMatch(Viewer viewer, Object element) {
                if (element instanceof ProjectFeature) {
                    // match in description and various other fields as well, no only in name
                    ProjectFeature f = (ProjectFeature) element;
                    String text =
                        f.getName() + " " + f.getId() + " " + StringUtils.join(f.getLabels(), " ") + " " +
                        f.getDescription() + " " + StringUtils.join(f.getNedPackages(), " ");
                    return wordMatches(text);
                }
                return super.isLeafMatch(viewer, element);
            }
        };
        filteredTree = new FilteredCheckboxTree(composite, SWT.CHECK | SWT.BORDER, patternFilter);
        treeViewer = (CheckboxTreeViewer) filteredTree.getViewer();
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)treeViewer.getTree().getLayoutData()).heightHint = convertHeightInCharsToPixels(16);

        // buttons area
        Composite buttonsArea = new Composite(composite, SWT.NONE);
        buttonsArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
        buttonsArea.setLayout(new GridLayout(1,false));
        Button selectAllButton = createButton(buttonsArea, SWT.PUSH, "&Select All", null);
        Button deselectAllButton = createButton(buttonsArea, SWT.PUSH, "&Deselect All", null);

        Label noteLabel = createLabel(composite, "");

        // register this page in the CDT property manager (note: if we skip this,
        // "Mark as source folder" won't work on the page until we visit a CDT property page)
        CDTPropertyManager.getProjectDescription(this, getProject());

        // the "Paths and Symbols" CDT page tends to display out-of-date info at the first
        // invocation; the following, seemingly no-op code apparently cures that...
        if (CDTPropertyManager.getProjectDescription(getProject()) != null)
            for (ICConfigurationDescription cfgDes : CDTPropertyManager.getProjectDescription(getProject()).getConfigurations())
                ManagedBuildManager.getConfigurationForDescription(cfgDes);  // the magic!

        // make the error text label wrap properly; see https://bugs.eclipse.org/bugs/show_bug.cgi?id=9866
        composite.addControlListener(new ControlAdapter(){
            public void controlResized(ControlEvent e){
                GridLayout layout = (GridLayout)composite.getLayout();
                GridData data = (GridData)errorMessageLabel.getLayoutData();
                data.widthHint = composite.getClientArea().width - 2*layout.marginWidth;
                GridData data2 = (GridData)bannerTextLabel.getLayoutData();
                data2.widthHint = composite.getClientArea().width - 2*layout.marginWidth;
                composite.layout(true);
            }
        });

        // configure "Fix it" actions on the error label
        errorMessageLabel.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                fixProblem(e.text);
                updateDiagnosticMessage();
            }
        });

        // configure the table
        treeViewer.setContentProvider(new ITreeContentProvider() {
            public void dispose() {
            }

            public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
            }

            public Object[] getElements(Object inputElement) {
                return getChildren(inputElement);
            }

            public Object[] getChildren(Object parentElement) {
                if (parentElement instanceof ProjectFeaturesManager)
                    return ((ProjectFeaturesManager) parentElement).getFeatures().toArray();
                else
                    return new Object[0];
            }

            public Object getParent(Object element) {
                return null; // apparently not needed in our case
            }

            public boolean hasChildren(Object element) {
                return (element instanceof ProjectFeaturesManager);
            }

        });
        treeViewer.setLabelProvider(new LabelProvider() {
            public String getText(Object element) {
                return ((ProjectFeature)element).getName();
            }
        });

        new HoverSupport().adapt(treeViewer.getTree(), new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = treeViewer.getTree().getItem(new Point(x,y));
                Object element = item==null ? null : item.getData();
                if (element instanceof ProjectFeature) {
                    String result = getHtmlFeatureInfo((ProjectFeature)element);
                    return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(result));
                }
                return null;
            }
        });

        treeViewer.addCheckStateListener(new ICheckStateListener() {
            public void checkStateChanged(CheckStateChangedEvent event) {
                ProjectFeature feature = (ProjectFeature)event.getElement();
                setFeatureEnabled(feature, event.getChecked());
            }
        });

        // configure the buttons
        selectAllButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setAllFeaturesEnabled(true);
            }
        });
        deselectAllButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setAllFeaturesEnabled(false);
            }
        });

        // load feature descriptions from file
        try {
            features = new ProjectFeaturesManager(getProject());
            features.loadFeaturesFile();
        }
        catch (CoreException e) {
            Activator.logError("Error reading feature description file for project " + getProject().getName(), e);
            errorDialog("Error reading feature description file", e);
        }

        treeViewer.setInput(features);
        if (features.getDefinesFile() != null)
            noteLabel.setText("Generated header file: "+ features.getDefinesFile().getFullPath().toOSString());

        // select enabled features in the checkbox table
        try {
            treeViewer.setCheckedElements(features.getEnabledFeatures().toArray());
        }
        catch (CoreException e) {
            Activator.logError("Cannot access saved feature enablement state for project " + getProject().getName(), e);
            errorDialog("Cannot access saved feature enablement state for project, using defaults", e);
            for (ProjectFeature f : features.getFeatures())
                treeViewer.setChecked(f, f.getInitiallyEnabled());
        }

        // load NED source folders configuration
        try {
            nedSourceFoldersConfig = NedResourcesPlugin.getNedSourceFoldersEditingManager().getConfig(getContainer(), getProject());
        }
        catch (CoreException e) {
            Activator.logError("Cannot read NED Source Folders list for project " + getProject().getName(), e);
            errorDialog("Cannot read NED Source Folders list: ", e);
            nedSourceFoldersConfig = new NedSourceFoldersConfiguration(new IContainer[0], new String[0]);
        }

        updateDiagnosticMessage();

        return composite;
    }

    protected Group createGroup(Composite composite, String text, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(numColumns,false));
        return group;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.WRAP);
        label.setText(text);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, true, false));
        return label;
    }

    protected Text createReadonlyText(Composite composite, String text) {
        return createReadonlyText(composite, text, 1);
    }

    protected Text createReadonlyText(Composite composite, String text, int numLines) {
        Text textWidget = new Text(composite, SWT.BORDER | SWT.READ_ONLY | (numLines>1 ? (SWT.MULTI | SWT.WRAP | SWT.V_SCROLL) : 0));
        textWidget.setText(text);
        textWidget.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, numLines>1));
        ((GridData)textWidget.getLayoutData()).heightHint = textWidget.getLineHeight()*numLines;
        return textWidget;
    }

    protected Button createButton(Composite parent, int style, String text, String tooltip) {
        Button button = new Button(parent, style);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        return button;
    }

    protected Link createLink(Composite composite, String text) {
        Link link = new Link(composite, SWT.NONE);
        link.setText(text);
        link.setLayoutData(new GridData());
        return link;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        if (visible == true)
            updateDiagnosticMessage();
    }

    protected String getHtmlFeatureInfo(ProjectFeature f) {
        String result = "<p><b>" + f.getName() + "</b></p>\n\n";
        String desc = StringUtils.nullToEmpty(f.getDescription()).trim();
        if (desc.contains("</p>") || desc.contains("</P>"))
            result += desc + "\n\n";
        else
            result += "<p>" + desc + "</p>\n\n";
        result += "<b>Details:</b><br>\n\n";
        result += makeLine("Id", f.getId(), false);
        result += makeLine("Requires", f.getDependencies(), false);
        result += makeLine("NED packages", f.getNedPackages(), false);
        result += makeLine("Extra C++ folders", f.getExtraSourceFolders(), true);
        result += makeLine("Compile flags", f.getCompileFlags(), true);
        result += makeLine("Linker flags", f.getLinkerFlags(), true);
        result += makeLine("Labels", f.getLabels(), true);
        result += makeLine("Initially enabled", f.getInitiallyEnabled() ? "true" : "false", false);
        return result;
    }

    protected static String makeLine(String name, Set<String> c, boolean skipIfEmpty) {
        return makeLine(name, StringUtils.join(c, ", "), skipIfEmpty);
    }

    protected static String makeLine(String name, String value, boolean skipIfEmpty) {
        if (StringUtils.isEmpty(value)) {
            if (skipIfEmpty)
                return "";
            value = "-";
        }
        return "<u>" + name + "</u>: " + value + "<br>\n";
    }

    protected void setAllFeaturesEnabled(boolean enable) {
        for (ProjectFeature f : features.getFeatures())
            if (treeViewer.getChecked(f) != enable)
                doSetFeatureEnabled(f, enable);
        updateDiagnosticMessage();
    }

    protected void setFeatureEnabled(ProjectFeature feature, boolean enable) {
        if (enable) {
            // collect missing dependencies
            List<ProjectFeature> missingDependencies = collectMissingDependencies(feature);

            // ask for confirmation
            if (!missingDependencies.isEmpty()) {
                String question = "The following feature(s) are required by " + quotedName(feature) + ", and will also be enabled:\n\n" + itemizeNamesOf(missingDependencies);
                if (!MessageDialog.openConfirm(getShell(), "Confirm Enabling Features", question)) {
                    treeViewer.setChecked(feature, !enable);  // restore original state
                    return;
                }
            }

            // do it
            doSetFeatureEnabled(feature, true);
            for (ProjectFeature f : missingDependencies)
                doSetFeatureEnabled(f, true);
        }
        else {
            // collect features that depend on the one we are turning off
            List<ProjectFeature> dependentFeatures = collectFeaturesThatDependOn(feature);

            // ask for confirmation
            if (!dependentFeatures.isEmpty()) {
                String question = "The following feature(s) require " + quotedName(feature) + ", and will also be disabled:\n\n" + itemizeNamesOf(dependentFeatures);
                if (!MessageDialog.openConfirm(getShell(), "Confirm Disabling Features", question)) {
                    treeViewer.setChecked(feature, !enable); // restore original state
                    return;
                }
            }

            // do it
            doSetFeatureEnabled(feature, false);
            for (ProjectFeature f : dependentFeatures)
                doSetFeatureEnabled(f, false);
        }

        updateDiagnosticMessage();
    }

    protected List<ProjectFeature> collectMissingDependencies(ProjectFeature feature) {
        Set<ProjectFeature> dependencies = features.collectDependencies(feature);
        List<ProjectFeature> missingDependencies = new ArrayList<ProjectFeature>();
        for (ProjectFeature f : dependencies)
            if (!treeViewer.getChecked(f))
                missingDependencies.add(f);
        return missingDependencies;
    }

    protected List<ProjectFeature> collectFeaturesThatDependOn(ProjectFeature feature) {
        Set<ProjectFeature> dependentFeatures = features.collectDependentFeatures(feature);
        List<ProjectFeature> bogusFeatures = new ArrayList<ProjectFeature>();
        for (ProjectFeature f : dependentFeatures)
            if (treeViewer.getChecked(f))
                bogusFeatures.add(f);
        return bogusFeatures;
    }


    protected void doSetFeatureEnabled(ProjectFeature feature, boolean enable) {
        treeViewer.setChecked(feature, enable);

        try {
            ICConfigurationDescription[] configurations = CDTPropertyManager.getProjectDescription(getProject()).getConfigurations();
            features.setFeatureEnabled(feature, enable, configurations, nedSourceFoldersConfig);
        }
        catch (Exception e) {
            errorDialog(e.getMessage(), e);
        }
    }

    protected static String itemizeNamesOf(Collection<ProjectFeature> features) {
        List<String> names = new ArrayList<String>();
        for (ProjectFeature f : features)
            names.add(" - " + f.getName());
        return StringUtils.join(names, "\n");
    }

    protected static String joinNamesOf(Collection<ProjectFeature> features) {
        List<String> names = new ArrayList<String>();
        for (ProjectFeature f : features)
            names.add(f.getName());
        return "\"" + StringUtils.join(names, "\", \"", "\" and \"") + "\"";
    }

    protected static String quotedName(ProjectFeature feature) {
        return "\"" + feature.getName() + "\"";
    }

    protected void updateDiagnosticMessage() {
        // compute diagnostic message
        String msg = getDiagnosticMessage();
        setDiagnosticMessage(msg);
    }

    protected String getDiagnosticMessage() {
        String result = "";

        // problems in .oppfeatures
        List<String> errors = features.validateFeatureDescriptions();
        if (!errors.isEmpty())
            result += "Problems were found in the feature description file. <a href=\"oppfeatures\">Details</a>\n";

        // check that all enabled features have their dependencies enabled
        List<ProjectFeature> bogusFeatures = new ArrayList<ProjectFeature>();
        for (ProjectFeature f : features.getFeatures()) {
            if (treeViewer.getChecked(f)) {
                List<ProjectFeature> missing = collectMissingDependencies(f);
                if (!missing.isEmpty())
                    bogusFeatures.add(f);
            }
        }
        if (!bogusFeatures.isEmpty())
            result += "Some features are missing dependencies. <a href=\"dependency\">Details</a>\n";

        // check that CDT and NED state corresponds to the feature selection
        try {
            List<Problem> problems = features.validateProjectState();
            if (!problems.isEmpty())
                result += "Some project settings do not correspond to the enabled features. <a href=\"config\">Details</a>\n";
        }
        catch (CoreException e) {
            Activator.logError(e);
        }

        return result.trim();
    }

    protected List<ProjectFeature> getEnabledFeaturesFromTree() {
        // Note: FilteredCheckboxTree's getCheckedElements() is bogus, cannot be used
        List<ProjectFeature> result = new ArrayList<ProjectFeature>();
        for (ProjectFeature f : features.getFeatures())
            if (treeViewer.getChecked(f))
                result.add(f);
        return result;
    }

    protected void setDiagnosticMessage(String message) {
        // Note: page already has error message functionality
        errorMessageLabel.setText(message==null ? "" : message);
        ((GridData)errorMessageLabel.getLayoutData()).exclude = (message==null);
        errorMessageLabel.setVisible(message!=null);
        errorMessageLabel.getParent().layout(true);
    }

    protected void fixProblem(String problemId) {
        // note: problemId comes from the "href" attribute of the "a" tag in the link's text: "<a href='problemId'>...</a>"
        if (problemId.equals("oppfeatures")) {
            List<String> problems = features.validateFeatureDescriptions();
            ProblemsMessageDialog.openInformation(getShell(),
                    "Project Setup Inconsistency",
                    "The following problems were found in " + features.getFeatureDescriptionFile().getFullPath() + ":\n",
                    problems, UIUtils.ICON_ERROR);
        }

        // "An enabled feature does not have all of its required features enabled. [Details]"
        if (problemId.equals("dependency")) {
            List<String> problems = new ArrayList<String>();
            for (ProjectFeature f : features.getFeatures()) {
                if (treeViewer.getChecked(f)) {
                    List<ProjectFeature> missing = collectMissingDependencies(f);
                    if (!missing.isEmpty())
                        problems.add(f.getName() + ": missing " + (missing.size()==1?"dependency":"dependencies") + " " + joinNamesOf(missing));
                }
            }
            ProblemsMessageDialog.openInformation(getShell(),
                    "Missing Dependencies",
                    "The following problems were found in the features selection (please fix them manually):\n",
                    problems, UIUtils.ICON_ERROR);
        }

        // "The project configuration does not correspond to the selected features. [Details]"
        if (problemId.equals("config")) {
            try {
                List<Problem> problems = features.validateProjectState();
                if (!problems.isEmpty() && isOkToFixConfigProblems(problems)) {
                    features.fixupProjectState();
                    NedSourceFoldersConfiguration tmp = ProjectUtils.readNedFoldersFile(getProject());
                    nedSourceFoldersConfig.setExcludedPackages(tmp.getExcludedPackages());
                }
            }
            catch (CoreException e1) {
                Activator.logError(e1);
            }
        }

    }

    protected boolean isOkToFixConfigProblems(List<Problem> problems) {
        List<String> problemTexts = new ArrayList<String>();
        for (Problem p : problems)
            problemTexts.add(p.toString());
        Shell parentShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        return ProblemsMessageDialog.openConfirm(parentShell, "Project Setup Inconsistency",
                "Some project configuration settings do not correspond to the enabled project features. " +
                "Do you want to fix the project state?",
                problemTexts, UIUtils.ICON_ERROR);
    }

    protected ProjectFeature getTableSelection() {
        Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        return element instanceof ProjectFeature ? (ProjectFeature)element : null;
    }

    /**
     * The resource for which the Properties dialog was brought up.
     */
    protected IProject getProject() {
        return (IProject) getElement().getAdapter(IProject.class);
    }

    @Override
    protected void performDefaults() {
        for (ProjectFeature f : features.getFeatures())
            if (treeViewer.getChecked(f) != f.getInitiallyEnabled())
                doSetFeatureEnabled(f, f.getInitiallyEnabled());
        updateDiagnosticMessage();
        super.performDefaults();
    }

    @Override
    public boolean performOk() {
        if (WorkspaceJob.getJobManager().find(ResourcesPlugin.FAMILY_MANUAL_BUILD).length != 0) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot update settings while build is in progress.");
            return false;
        }
        // save .nedfolders
        try {
            ProjectUtils.saveNedFoldersFile(getProject(), nedSourceFoldersConfig);
        }
        catch (CoreException e) {
            Activator.logError("Cannot store updated NED Source Folders configuration for project " + getProject().getName(), e);
            errorDialog("Cannot store updated NED Source Folders configuration for project", e);
            return false;
        }

        // save features enablement state and generated header
        try {
            List<ProjectFeature> enabledFeatures = getEnabledFeaturesFromTree();
            features.saveFeatureEnablement(enabledFeatures);
            if (features.getDefinesFile() != null)
                features.saveDefinesFile(enabledFeatures);
        }
        catch (CoreException e) {
            Activator.logError("Cannot store feature enablement state for project " + getProject().getName(), e); //XXX msg
            errorDialog("Cannot store feature enablement state for project", e);//XXX msg
            return false;
        }

        CDTPropertyManager.performOkForced(this); // performOk() doesn't always save it...
        CDTUtils.invalidateDiscoveredPathInfo(getProject());

        return true;
    }

    protected void errorDialog(String message, Throwable e) {
        Activator.logError(message, e);
        IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
        ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
    }

}

